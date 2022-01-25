#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "common.h"
#include "logger.h"
#include "configmanager.h"
#include "transform.h"
#include "protocol.h"
#include "systick.h"
#include "sysrtc.h"
#include "i2c.h"
#include "hmc5883l.h"
#include "adxl345.h"

#define RX_BUFFER_SIZE	64
#define TX_BUFFER_SIZE	48

int8_t logBuffer[LOG_BUFFER_SIZE];
uint8_t usbControlBuffer[128];

uint8_t cdcBufferRX[RX_BUFFER_SIZE];
uint8_t cdcBufferTX[TX_BUFFER_SIZE];

static volatile uint8_t usbInit;
static volatile USBMsgState msgState;

// USB CDC descriptors.

static const struct usb_device_descriptor devDescriptor = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = USB_CLASS_CDC,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5740,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const struct usb_endpoint_descriptor endpointCommunication[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x83,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = 16,
	.bInterval = 255,
}};

static const struct usb_endpoint_descriptor endpointData[] = {{
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}, {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
}};

static const struct {
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor callMgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdcUnion;
} __attribute__((packed)) cdcFunctionDescriptor = {
	.header = {
		.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
		.bcdCDC = 0x0110,
	},
	.callMgmt = {
		.bFunctionLength = 
			sizeof(struct usb_cdc_call_management_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
		.bmCapabilities = 0,
		.bDataInterface = 1,
	},
	.acm = {
		.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_ACM,
		.bmCapabilities = 0,
	},
	.cdcUnion = {
		.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
		.bDescriptorType = CS_INTERFACE,
		.bDescriptorSubtype = USB_CDC_TYPE_UNION,
		.bControlInterface = 0,
		.bSubordinateInterface0 = 1, 
	}
};

static const struct usb_interface_descriptor intfCommunication[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_CDC,
	.bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
	.bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
	.iInterface = 0,

	.endpoint = endpointCommunication,

	.extra = &cdcFunctionDescriptor,
	.extralen = sizeof(cdcFunctionDescriptor)
}};

static const struct usb_interface_descriptor intfData[] = {{
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,

	.endpoint = endpointData,
}};

static const struct usb_interface intfUSB[] = {{
	.num_altsetting = 1,
	.altsetting = intfCommunication,
}, {
	.num_altsetting = 1,
	.altsetting = intfData,
}};

static const struct usb_config_descriptor usbConfigDescriptor = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 2,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0x80,
	.bMaxPower = 0x32,

	.interface = intfUSB,
};

static const char *usbStr[] = {
	"Dilshan R Jayakody",
	"Star Pointer",
	"SP0001",
};

static uint8_t isSettingsCommand(uint8_t *inBuffer, const uint8_t *matchStr)
{
	if((strlen(inBuffer) > 4) && (strlen(matchStr) >= 2))
	{
		// Check for valid command header.
		if((inBuffer[0] == '#') && (inBuffer[1] == ':'))
		{
			// Match command ID.
			return ((inBuffer[2] == matchStr[0]) && (inBuffer[3] == matchStr[1])) ? SUCCESS : FAIL;
		}
	}

	return FAIL;
}

static enum usbd_request_return_codes cdcacm_control_request(usbd_device *usbd_dev, struct usb_setup_data *req, uint8_t **buf,
    uint16_t *len, void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch(req->bRequest) 
    {
        case USB_CDC_REQ_SET_CONTROL_LINE_STATE: 
        {
            char local_buf[10];
            struct usb_cdc_notification *notif = (void *)local_buf;

            notif->bmRequestType = 0xA1;
            notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
            notif->wValue = 0;
            notif->wIndex = 0;
            notif->wLength = 2;
            local_buf[8] = req->wValue & 3;
            local_buf[9] = 0;

            return USBD_REQ_HANDLED;
        }
        case USB_CDC_REQ_SET_LINE_CODING: 
            if(*len < sizeof(struct usb_cdc_line_coding))
                return USBD_REQ_NOTSUPP;

            return USBD_REQ_HANDLED;
	}

	return USBD_REQ_NOTSUPP;
}

static void cdcDataReceiveCallback(usbd_device *usbd_dev, uint8_t ep)
{
	int rxSize;
	
	(void)ep;

	// Reset CDC RX and TX buffers.
	memset(cdcBufferRX, 0, RX_BUFFER_SIZE);
	memset(cdcBufferTX, 0, TX_BUFFER_SIZE);

	rxSize = usbd_ep_read_packet(usbd_dev, 0x01, cdcBufferRX, RX_BUFFER_SIZE);
	if(rxSize > 0)
	{
		if(strcmp(cdcBufferRX, "#:GR#") == 0)
		{
			// Get RA from the sensor array.
			msgState = MSG_GET_RA;
		}
		else if(strcmp(cdcBufferRX, "#:GD#") == 0)
		{
			// Get DEC from the sensor array.
			msgState = MSG_GET_DEC;
		}
		else if(isSettingsCommand(cdcBufferRX, "SC") == SUCCESS)
		{
			// Set UTC date.
			msgState = MSG_SET_DATE;
		}
		else if(isSettingsCommand(cdcBufferRX, "SL") == SUCCESS)
		{
			// Set UTC time.
			msgState = MSG_SET_TIME;
		}
		else if(isSettingsCommand(cdcBufferRX, "St") == SUCCESS)
		{
			// Set latitdue of the current site.
			msgState = MSG_SET_LAT;
		}
		else if(isSettingsCommand(cdcBufferRX, "Sg") == SUCCESS)
		{
			// Set longitude of the current site.
			msgState = MSG_SET_LNG;
		}
		else if(isSettingsCommand(cdcBufferRX, "Sm") == SUCCESS)
		{
			// Set magnetic declination offset.
			msgState = MSG_SET_MAG_OFFSET;
		}
		else if(isSettingsCommand(cdcBufferRX, "Sv") == SUCCESS)
		{
			// Set inclination offset.
			msgState = MSG_SET_INCL_OFFSET;
		}
	}
}

static void cdcSetConfig(usbd_device *usbd_dev, uint16_t wValue)
{
    (void)wValue;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcDataReceiveCallback);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        cdcacm_control_request);

	// Finish initializing USB CDC interface.
	usbInit = false;
}

static void initSystem()
{
    // External osciliator mode with 72MHz internal clocking.
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    // Enable clocking on used peripherals.
    rcc_periph_clock_enable(RCC_AFIO);    
    rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_USART2);
	rcc_periph_clock_enable(RCC_USB);
	rcc_periph_clock_enable(RCC_I2C2);
	rcc_periph_clock_enable(RCC_PWR);

    AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;
}

static void setMessageResponse(usbd_device *usbDevice, uint8_t *resp)
{
	usbd_ep_write_packet(usbDevice, 0x82, cdcBufferTX, strlen(cdcBufferTX));
	msgState = MSG_IDLE;

	LOG(cdcBufferTX);
}

int main(void)
{
    usbd_device *usbDevice;
	uint32_t sysTickTempVal;
	float compassHeading, tiltX, tiltY;	
	float accAngle;
	float latitude, longitude;
	float floatTime;
	float resRA, resDEC;
	float locationDecCorrection;
	float inclination;
	CompassData compassData;
	AccelerometerData accData;	
	struct tm sysTime;
	Angle angleRA, angleDEC;
	int8_t decSign;

	// Reset all global variables.
	msgState = MSG_IDLE;

    // Initialize system components and core peripherals.
    initSystem();
	initSysTick();
    initLogger();
	
    // Start Initializing firmware.
    LOG("Initializing the system...");

	// SysTick verification with 50ms delay.
	systickDelay(50);
	LOG("SysTick verification completed");

	// Initialize I2C - 2.
	LOG("Initialize I2C interface 2");
	initI2C();	

	// Initialize HMC5883L magnetometer.
	LOG("Initialize magnetometer");
	initMagnetometer();

	LOG("Initialize accelerometer");
	if(initAccelerometer() == FAIL)
	{
		LOG("Accelerometer initialization failed");
	}

	LOG("Loading user configuration");
	// Loading user/location specific configuration data.
	locationDecCorrection = getLocationDecAngle();
	inclination = getInclinationOffset();
	getLocationLatLng(&latitude, &longitude);

	LOG("Latitdue : %f", latitude);
	LOG("Longitude : %f", longitude);

	LOG("Offset MAG: %f", locationDecCorrection);
	LOG("Offset INC: %f", inclination);

	// Initialize system clock.
	LOG("Initialize RTC");
	initRTC();

	getSystemDateTime(&sysTime);
	LOG_TIME(sysTime);
	LOG_DATE(sysTime);

	// Initialize USB CDC device (to provide the LX200 gateway).
	LOG("Setup USB CDC interface");
	usbInit = true;
    usbDevice = usbd_init(&st_usbfs_v1_usb_driver, &devDescriptor, &usbConfigDescriptor, usbStr, 3, usbControlBuffer, sizeof(usbControlBuffer));
    usbd_register_set_config_callback(usbDevice, cdcSetConfig);

    // Start the main service loop.
    while(1)
    {
        // Handle pending USB messages.
		usbd_poll(usbDevice);
		if(usbInit)
		{
			// Allow time to initialize the USB CDC interface.
			continue;
		}
		
		// Obtain values from magnetometer and accelerometer.
		readMagnetometerNormalize(&compassData);
		usbd_poll(usbDevice);

		readAccelerometerScaled(&accData);
		usbd_poll(usbDevice);
		
		// Process magnetometer data to get RA.
		compassHeading = fixTiltCompensate(&compassData, &accData);
		compassHeading = fixAngle(compassHeading) * 180/PI;

		// Process accelerometer data to get DEC.
		accAngle = getPitchFromAccelerometer(&accData);

		// Get system time (in GMT) from RTC.
		getSystemDateTime(&sysTime);
		floatTime = convertTimeToFloat(&sysTime);

		// Find RA and DEC values of the current position.
		convertAzAltToRaDec(accAngle, compassHeading, sysTime.tm_year, sysTime.tm_mon, sysTime.tm_mday, 
			floatTime, latitude, longitude, &resRA, &resDEC);

		convertAngleToInt(resRA, &angleRA);
		convertAngleToInt(resDEC, &angleDEC);
		
		// Handle USB requests received from host.
		if(msgState == MSG_GET_RA)
		{
			// Return current right ascension of the sensor kit.
			LOG("MSG: Get RA");

			sprintf(cdcBufferTX, "%02d:%02d:%02d#", angleRA.deg, angleRA.min, angleRA.sec);
			setMessageResponse(usbDevice, cdcBufferTX);			
		}
		else if(msgState == MSG_GET_DEC)
		{
			// Return current declination of the sensor kit.
			LOG("MSG: Get DEC");

			decSign = (angleDEC.deg < 0) ? '-' : '+';
			sprintf(cdcBufferTX, "%c%02d*%02d:%02d#", decSign, abs(angleDEC.deg), angleDEC.min, angleDEC.sec);
			setMessageResponse(usbDevice, cdcBufferTX);	
		}
		else if(msgState == MSG_SET_DATE)
		{
			// Configuration change - Set date of the sensor unit.
			LOG("MSG: Set Date");

			// Extract value and update RTC.
			extractDateInfo(cdcBufferRX, &sysTime);			
			setSystemDateTime(&sysTime);

			LOG_DATE(sysTime);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);
		}
		else if(msgState == MSG_SET_TIME)
		{
			// Configuration change - Set time of the sensor unit.
			LOG("MSG: Set Time");

			// Extract value and update RTC.
			extractTimeInfo(cdcBufferRX, &sysTime);			
			setSystemDateTime(&sysTime);

			LOG_TIME(sysTime);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);
		}
		else if(msgState == MSG_SET_LAT)
		{
			// Configuration change - Set latitdue of the current site.
			LOG("MSG: Set Latitdue");

			latitude = extractAngle(cdcBufferRX);
			setLocationLatLng(latitude, longitude);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);

			LOG("Latitdue : %f", latitude);
		}
		else if(msgState == MSG_SET_LNG)
		{
			// Configuration change - Set longitude of the current site.
			LOG("MSG: Set Longitude");	

			longitude = extractAngle(cdcBufferRX);
			setLocationLatLng(latitude, longitude);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);

			LOG("Longitude : %f", longitude);
		}
		else if(msgState == MSG_SET_MAG_OFFSET)
		{
			// Configuration change - Set magnetic declination offset of the current site.
			LOG("MSG: Set MAG DEC");

			locationDecCorrection = extractAngle(cdcBufferRX);
			setLocationDecAngle(locationDecCorrection);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);

			LOG("MAG DEC : %f", locationDecCorrection);
		}
		else if(msgState == MSG_SET_INCL_OFFSET)
		{
			// Configuration change - Set inclination offset of the current site.
			LOG("MSG: Set INC OFFSET");

			inclination = extractAngle(cdcBufferRX);
			setInclinationOffset(inclination);

			// Send successful response.
			sprintf(cdcBufferTX, "1");
			setMessageResponse(usbDevice, cdcBufferTX);

			LOG("INC OFFSET : %f", inclination);
		}
    }
    
    return 0;
}