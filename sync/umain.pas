unit umain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls, ExtCtrls,
  IniFiles, ucommon, DateUtils, Math;

type

  { TfrmStarPointerSync }

  TfrmStarPointerSync = class(TForm)
    btnSync: TButton;
    btnCancel: TButton;
    chkSysDateTime: TCheckBox;
    cmbCountry: TComboBox;
    cmbCity: TComboBox;
    cmbState: TComboBox;
    cmbUTCOffset: TComboBox;
    pnlButton: TPanel;
    lblState: TStaticText;
    lblUTCOffset: TStaticText;
    tmrSysTime: TTimer;
    txtDevPath: TEdit;
    grpDevice: TGroupBox;
    lblDevPath: TStaticText;
    txtDecOffset: TEdit;
    txtInclOffset: TEdit;
    grpOffset: TGroupBox;
    lblGMTValue: TLabel;
    lblGMT: TStaticText;
    lblDecOffset: TStaticText;
    lblInclOffset: TStaticText;
    txtSeconds: TEdit;
    txtMinutes: TEdit;
    txtYear: TEdit;
    txtMonth: TEdit;
    txtDate: TEdit;
    grpTime: TGroupBox;
    lblCountry: TStaticText;
    lblCity: TStaticText;
    lblTime: TStaticText;
    lblDate: TStaticText;
    txtLng: TEdit;
    txtLat: TEdit;
    grpLocation: TGroupBox;
    lblLat: TStaticText;
    lblLng: TStaticText;
    txtHour: TEdit;
    procedure btnCancelClick(Sender: TObject);
    procedure btnSyncClick(Sender: TObject);
    procedure chkSysDateTimeChange(Sender: TObject);
    procedure cmbCitySelect(Sender: TObject);
    procedure cmbCountrySelect(Sender: TObject);
    procedure cmbStateSelect(Sender: TObject);
    procedure cmbUTCOffsetSelect(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure tmrSysTimeTimer(Sender: TObject);
  private
    countryInfo : TList;
    isSkipCountryListHeader: boolean;

    function CountryListFileName() : string;
    function UTCOffsetListFileName() : string;

    procedure LoadCountryList();
    procedure ParseCountyInfo(const currentValue: string);

    procedure SetCountryList();
    procedure SetStateList(country: string);
    procedure SetCityList(country: string; state: string);
    procedure GetLocationCoordinates(country: string; state: string; city: string);

    procedure SetUTCOffsetList();
    procedure ParseTZInfo(const currentValue: string);
    function GetUTC(inTime: TDateInfo): TDateTime;
    function UpdateDateTime() : TDateInfo;
    procedure UpdateSysDateTime();

    procedure SaveUserConfig(conf: TUserConfig);
    function LoadUserConfig() : TUserConfig;
  public

  end;

var
  frmStarPointerSync: TfrmStarPointerSync;

implementation

{$R *.lfm}

{ TfrmStarPointerSync }

function TfrmStarPointerSync.CountryListFileName() : string;
var
  confgFile: TIniFile;
begin
  confgFile := TIniFile.Create(CONFIG_FILE_NAME);
  result := confgFile.ReadString('countrylist', 'filename', 'worldcities.csv');
  FreeAndNil(confgFile);
end;

function TfrmStarPointerSync.UTCOffsetListFileName() : string;
var
  confgFile: TIniFile;
begin
  confgFile := TIniFile.Create(CONFIG_FILE_NAME);
  result := confgFile.ReadString('utclist', 'filename', 'utc-offset');
  FreeAndNil(confgFile);
end;

procedure TfrmStarPointerSync.ParseCountyInfo(const currentValue: string);
var
  ptrCountyInfo : PCountyInfo;
  entryData: TStringArray;
begin
  // Skip first row (header section) of the county list file.
  if(isSkipCountryListHeader) then
  begin
    isSkipCountryListHeader := false;
    Exit;
  end;

  // Split each city info string into an array.
  entryData := CurrentValue.Split(',', '"', '"');

  // Extract required city information from returned array.
  New(ptrCountyInfo);
  with ptrCountyInfo^ do
  begin
    City := StringReplace(entryData[1], '"', '', [rfReplaceAll]);
    Country := StringReplace(entryData[4], '"', '', [rfReplaceAll]);
    State := StringReplace(entryData[7], '"', '', [rfReplaceAll]);
    Lat := StrToFloat(StringReplace(entryData[2], '"', '', [rfReplaceAll]));
    Lng := StrToFloat(StringReplace(entryData[3], '"', '', [rfReplaceAll]));
  end;

  // Add city information into the country-info list.
  countryInfo.Add(ptrCountyInfo);
end;

procedure TfrmStarPointerSync.LoadCountryList();
var countryFile: TStringList;
begin
  countryInfo := TList.Create;
  isSkipCountryListHeader := true;

  // Load country list file configured in starpointersync.ini.
  countryFile := TStringList.Create;
  countryFile.LoadFromFile(CountryListFileName());

  // Add each country into the countryInfo list.
  if(countryFile.Count > 0) then
  begin
    countryFile.ForEach(@ParseCountyInfo);
  end;

  // Release country list file.
  FreeAndNil(countryFile);
end;

procedure TfrmStarPointerSync.SetCountryList();
var
  cntPos: Integer;
  cntName: string;
begin
  cmbCountry.Items.Clear;
  cmbCountry.Sorted := false;

  // Scan country info list and add all available countries to the drop down.
  for cntPos := 0 to (countryInfo.Count - 1) do
  begin
    cntName := Trim(PCountyInfo(countryInfo[cntPos])^.Country);
    if(cmbCountry.Items.IndexOf(cntName) < 0) then
    begin
      cmbCountry.Items.Add(cntName);
    end;
  end;

  // Arrange country list.
  cmbCountry.Sorted := true;
  cmbCountry.DropDownCount := 10;
end;

procedure TfrmStarPointerSync.SetStateList(country: string);
var
  statePos: Integer;
  stateName: string;
begin
  cmbState.Items.Clear;
  cmbState.Sorted := false;

  screen.Cursor := crHourGlass;
  self.Invalidate;

  if(country <> '') then
  begin
    for statePos := 0 to (countryInfo.Count - 1) do
    begin
      // Get state list of the specified country.
      if(Trim(PCountyInfo(countryInfo[statePos])^.Country) = country) then
      begin
        stateName := Trim(PCountyInfo(countryInfo[statePos])^.State);
        if(cmbState.Items.IndexOf(stateName) < 0) then
        begin
          cmbState.Items.Add(stateName);
        end;
      end;
    end;
  end;

  cmbState.Sorted := true;
  Screen.Cursor := crDefault;
  self.Invalidate;
end;

procedure TfrmStarPointerSync.SetCityList(country: string; state: string);
var
  cityPos: Integer;
begin
  cmbCity.Items.Clear;
  cmbCity.Sorted := false;

  screen.Cursor := crHourGlass;
  self.Invalidate;

  if(country <> '') then
  begin
    for cityPos := 0 to (countryInfo.Count - 1) do
    begin
      // Get list of cities of the specified country and state.
      if((Trim(PCountyInfo(countryInfo[cityPos])^.Country) = country)
        and (Trim(PCountyInfo(countryInfo[cityPos])^.State) = state)) then
      begin
        cmbCity.Items.Add(Trim(PCountyInfo(countryInfo[cityPos])^.City));
      end;
    end;
  end;

  cmbCity.Sorted := true;
  Screen.Cursor := crDefault;
  self.Invalidate;
end;

procedure TfrmStarPointerSync.GetLocationCoordinates(country: string; state: string; city: string);
var
  locPos: Integer;
begin
  for locPos := 0 to (countryInfo.Count - 1) do
  begin
    // Fill longitude and latitude based on the specified location data.
    if((Trim(PCountyInfo(countryInfo[locPos])^.Country) = country)
      and (Trim(PCountyInfo(countryInfo[locPos])^.State) = state)
      and (Trim(PCountyInfo(countryInfo[locPos])^.City) = city)) then
    begin
      txtLat.Text := FloatToStr(PCountyInfo(countryInfo[locPos])^.Lat);
      txtLng.Text := FloatToStr(PCountyInfo(countryInfo[locPos])^.Lng);
      break;
    end;
  end;
end;

procedure TfrmStarPointerSync.ParseTZInfo(const currentValue: string);
begin
  cmbUTCOffset.Items.Add(Trim(currentValue));
end;

procedure TfrmStarPointerSync.SetUTCOffsetList();
var
  utcOffsetFile: TStringList;
begin
  cmbUTCOffset.Items.Clear;

  utcOffsetFile := TStringList.Create;
  utcOffsetFile.LoadFromFile(UTCOffsetListFileName());
  utcOffsetFile.ForEach(@ParseTZInfo);
  FreeAndNil(utcOffsetFile);
end;

procedure TfrmStarPointerSync.FormShow(Sender: TObject);
var
  userConf: TUserConfig;
begin
  try
     // Loading country list and UTC offset list from files.
     SetCountryList();
     SetUTCOffsetList();

     // Restore last user confuguration.
     userConf := LoadUserConfig();

     // Set location specific fields.
     cmbCountry.ItemIndex := cmbCountry.Items.IndexOf(userConf.LocationInfo.Country);
     cmbCountrySelect(Sender);

     cmbState.ItemIndex := cmbState.Items.IndexOf(userConf.LocationInfo.State);
     cmbStateSelect(Sender);

     cmbCity.ItemIndex := cmbCity.Items.IndexOf(userConf.LocationInfo.City);
     cmbCitySelect(Sender);

     txtLat.Text := FloatToStr(userConf.LocationInfo.Lat);
     txtLng.Text := FloatToStr(userConf.LocationInfo.Lng);

     // Set sensor specific fields.
     txtDecOffset.Text := FloatToStr(userConf.MagDecOffset);
     txtInclOffset.Text := FloatToStr(userConf.InclinOffset);

     // Set Device name/path field.
     txtDevPath.Text := userConf.DevPath;

     // Set date/time fields.
     cmbUTCOffset.ItemIndex:= cmbUTCOffset.Items.IndexOf(userConf.UTCOffset);
     UpdateSysDateTime();

     chkSysDateTime.Checked := userConf.SysTime;
     chkSysDateTimeChange(Sender);
  except
    on E:Exception do
    begin
      MessageDlg(Application.Title, E.Message, TMsgDlgType.mtError, [TMsgDlgBtn.mbOK], 0);
    end;
  end;
end;

procedure TfrmStarPointerSync.tmrSysTimeTimer(Sender: TObject);
begin
  UpdateSysDateTime();
end;

procedure TfrmStarPointerSync.FormCreate(Sender: TObject);
begin
  LoadCountryList();
end;

procedure TfrmStarPointerSync.btnCancelClick(Sender: TObject);
begin
  Close;
end;

procedure TfrmStarPointerSync.btnSyncClick(Sender: TObject);
var
  cnf: TUserConfig;
  userDateTime: TDateInfo;
  tempVal : string;
  tempFloat: Extended;
  tempNum: Integer;
begin
  try
    // Perform validations on user specified location data.

    if(cmbCountry.Text = '') then
    begin
      // Country is not selected!
      raise Exception.Create(ERR_COUNTRY_NOT_DEFINED);
    end;

    cnf.LocationInfo.Country := cmbCountry.Text;

    if(cmbState.Text = '') then
    begin
      // State is not selected!
      raise Exception.Create(ERR_STATE_NOT_DEFINED);
    end;

    cnf.LocationInfo.State := cmbState.Text;

    if(cmbCity.Text = '') then
    begin
      // City is not selected!
      raise Exception.Create(ERR_CITY_NOT_DEFINED);
    end;

    cnf.LocationInfo.City := cmbCity.Text;

    // Validate Latitude.

    tempVal := trim(txtLat.Text);
    if(tempVal = '') then
    begin
      // Latitude is not specified!
      raise Exception.Create(ERR_LAT_NOT_DEFINED);
    end;

    tempFloat := StrToFloatDef(tempVal, NaN);
    if(IsNan(tempFloat)) then
    begin
      // Invalid latitude value!
      raise Exception.Create(ERR_INVALID_LAT);
    end;

    cnf.LocationInfo.Lat := tempFloat;

    // Validate Longitude.

    tempVal := trim(txtLng.Text);
    if(tempVal = '') then
    begin
      // Longitude is not specified!
      raise Exception.Create(ERR_LNG_NOT_DEFINED);
    end;

    tempFloat := StrToFloatDef(tempVal, NaN);
    if(IsNan(StrToFloatDef(tempVal, NaN))) then
    begin
      // Invalid longitude value!
      raise Exception.Create(ERR_INVALID_LNG);
    end;

    cnf.LocationInfo.Lng := tempFloat;

    // Validate magnetic declination offset.

    tempVal := trim(txtDecOffset.Text);
    if(tempVal = '') then
    begin
      // Magnetic declination offset is not specified!
      raise Exception.Create(ERR_MAG_DEC_NOT_DEFINED);
    end;

    tempFloat := StrToFloatDef(tempVal, NaN);
    if(IsNan(StrToFloatDef(tempVal, NaN))) then
    begin
      // Invalid magnetic declination offset value!
      raise Exception.Create(ERR_INVALID_MAG_DEC);
    end;

    cnf.MagDecOffset := tempFloat;

    // Validate inclination offset.

    tempVal := trim(txtInclOffset.Text);
    if(tempVal = '') then
    begin
      // Inclination offset is not specified!
      raise Exception.Create(ERR_INCL_OFFSET_NOT_DEFINED);
    end;

    tempFloat := StrToFloatDef(tempVal, NaN);
    if(IsNan(StrToFloatDef(tempVal, NaN))) then
    begin
      // Invalid inclination offset offset value!
      raise Exception.Create(ERR_INVALID_INCL_OFFSET);
    end;

    cnf.InclinOffset := tempFloat;

    // Validate UTC offset.

    if(cmbUTCOffset.Text = '') then
    begin
      // UTC offset is not selected!
      raise Exception.Create(ERR_UTC_OFFSET_NOT_DEFINED);
    end;

    cnf.UTCOffset := cmbUTCOffset.Text;
    cnf.SysTime := chkSysDateTime.Checked;

    // Validate device name/path;

    tempVal := Trim(txtDevPath.Text);
    if(tempVal = '') then
    begin
      // Device path/name is not specified!
      raise Exception.Create(ERR_DEV_PATH_NOT_DEFINDED);
    end;

    cnf.DevPath := tempVal;

    // Perform year validations.

    tempVal := Trim(txtYear.Text);
    if(tempVal = '') then
    begin
      // Year is not specified!
      raise Exception.Create(ERR_YEAR_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < MIN_YEAR) or (tempNum > MAX_YEAR)) then
    begin
      // Invalid year value!
      raise Exception.Create(ERR_INVALID_YEAR);
    end;

    userDateTime.Year := tempNum;

    // Perform month validations.

    tempVal := Trim(txtMonth.Text);
    if(tempVal = '') then
    begin
      // Month is not specified!
      raise Exception.Create(ERR_MONTH_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < 1) or (tempNum > 12)) then
    begin
      // Invalid month value!
      raise Exception.Create(ERR_INVALID_MONTH);
    end;

    userDateTime.Month := tempNum;

    // Perform day validations.

    tempVal := Trim(txtDate.Text);
    if(tempVal = '') then
    begin
      // Day is not specified!
      raise Exception.Create(ERR_DAY_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < 1) or (tempNum > 31)) then
    begin
      // Invalid day value!
      raise Exception.Create(ERR_INVALID_DAY);
    end;

    userDateTime.Day := tempNum;

    // Perform hour validations.

    tempVal := Trim(txtHour.Text);
    if(tempVal = '') then
    begin
      // Hour is not specified!
      raise Exception.Create(ERR_HOUR_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < 0) or (tempNum > 23)) then
    begin
      // Invalid hour value!
      raise Exception.Create(ERR_INVALID_HOUR);
    end;

    userDateTime.Hour := tempNum;

    // Perform minutes validations.

    tempVal := Trim(txtMinutes.Text);
    if(tempVal = '') then
    begin
      // Minutes is not specified!
      raise Exception.Create(ERR_MINUTE_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < 0) or (tempNum > 59)) then
    begin
      // Invalid minutes value!
      raise Exception.Create(ERR_INVALID_MINUTE);
    end;

    userDateTime.Minutes := tempNum;

    // Perform seconds validations.

    tempVal := Trim(txtSeconds.Text);
    if(tempVal = '') then
    begin
      // Seconds is not specified!
      raise Exception.Create(ERR_SECONDS_NOT_DEFINED);
    end;

    tempNum := StrToIntDef(tempVal, MaxInt);
    if((tempNum = MaxInt) or (tempNum < 0) or (tempNum > 59)) then
    begin
      // Invalid seconds value!
      raise Exception.Create(ERR_INVALID_SECONDS);
    end;

    userDateTime.Seconds := tempNum;

    // Save user inputs into the profile.
    SaveUserConfig(cnf);

  except
    on E:Exception do
    begin
      MessageDlg(Application.Title, E.Message, TMsgDlgType.mtError, [TMsgDlgBtn.mbOK], 0);
    end;
  end;
end;

procedure TfrmStarPointerSync.chkSysDateTimeChange(Sender: TObject);
begin
  txtYear.Enabled := not chkSysDateTime.Checked;
  txtMonth.Enabled := txtYear.Enabled;
  txtDate.Enabled := txtYear.Enabled;
  txtHour.Enabled := txtYear.Enabled;
  txtMinutes.Enabled := txtYear.Enabled;
  txtSeconds.Enabled := txtYear.Enabled;

  // Enable or disable system timer.
  tmrSysTime.Enabled := chkSysDateTime.Checked;

  if(chkSysDateTime.Checked)then
  begin
    // Refresh time controls immediately with system time.
    tmrSysTimeTimer(Sender);
  end;
end;

procedure TfrmStarPointerSync.cmbCitySelect(Sender: TObject);
begin
  GetLocationCoordinates(cmbCountry.Text, cmbState.Text, cmbCity.Text);
end;

procedure TfrmStarPointerSync.cmbCountrySelect(Sender: TObject);
begin
  SetStateList(cmbCountry.Text);
  cmbCity.Clear;
end;

procedure TfrmStarPointerSync.cmbStateSelect(Sender: TObject);
begin
  SetCityList(cmbCountry.Text, cmbState.Text);
end;

procedure TfrmStarPointerSync.cmbUTCOffsetSelect(Sender: TObject);
begin
  UpdateDateTime();
end;

procedure TfrmStarPointerSync.FormDestroy(Sender: TObject);
var
  cntInfoPos : integer;
  cntRec: PCountyInfo;
begin
  if(countryInfo.Count > 0) then
  begin
    // Release all city entries in the countryInfo list.
    for cntInfoPos := 0 to (countryInfo.Count - 1) do
    begin
      cntRec := countryInfo[cntInfoPos];
      Dispose(cntRec);
    end;
  end;

  // Release all dynamically created objects.
  FreeAndNil(countryInfo);
end;

function TfrmStarPointerSync.GetUTC(inTime: TDateInfo): TDateTime;
var
  utcOffset: string;
  usrTime, offset: TDateTime;
  offsetHour, offsetMinute : Word;
  utcOffsetPlus : boolean;
begin
  // Get UTC offset selection.
  utcOffset := cmbUTCOffset.Text;

  if(utcOffset <> '') then
  begin
    utcOffsetPlus := (utcOffset[1] = '+');
    offsetHour := StrToInt(Copy(utcOffset, 2, 2));
    offsetMinute := StrToInt(Copy(utcOffset, 5, 2));
  end
  else
  begin
    // UTC offset is not selected!
    offsetHour := 0;
    offsetMinute := 0;
    utcOffsetPlus := false;
  end;
  offset := EncodeTime(offsetHour, offsetMinute, 0, 0);

  // User specifed time.
  usrTime := EncodeDateTime(inTime.Year, inTime.Month, inTime.Day,
    inTime.Hour, inTime.Minutes, inTime.Seconds, 0);

  // Get UTC time.
  if(utcOffsetPlus)then
  begin
    // UTC+X offset
    usrTime := usrTime - offset;
  end
  else
  begin
    // UTC-X offset
    usrTime := usrTime + offset;
  end;

  result := usrTime;
end;

procedure TfrmStarPointerSync.SaveUserConfig(conf: TUserConfig);
var
  confgFile: TIniFile;
begin
  confgFile := TIniFile.Create(CONFIG_FILE_NAME);

  // Write location specific configuration.
  confgFile.WriteString('profile', 'country', conf.LocationInfo.Country);
  confgFile.WriteString('profile', 'state', conf.LocationInfo.State);
  confgFile.WriteString('profile', 'city', conf.LocationInfo.City);
  confgFile.WriteFloat('profile', 'lat', conf.LocationInfo.Lat);
  confgFile.WriteFloat('profile', 'lng', conf.LocationInfo.Lng);

  // Write sensor offset values.
  confgFile.WriteFloat('profile', 'magdecoffset', conf.MagDecOffset);
  confgFile.WriteFloat('profile', 'incloffset', conf.InclinOffset);

  // Write date/time related settings.
  confgFile.WriteString('profile', 'utcoffset', conf.UTCOffset);
  confgFile.WriteBool('profile', 'systime', conf.SysTime);

  // Write device path/name.
  confgFile.WriteString('profile', 'devpath', conf.DevPath);

  confgFile.UpdateFile;
  FreeAndNil(confgFile);
end;

function TfrmStarPointerSync.LoadUserConfig() : TUserConfig;
var
  confgFile: TIniFile;
  retData: TUserConfig;
begin
  confgFile := TIniFile.Create(CONFIG_FILE_NAME);

  // Read location specific configuration.
  retData.LocationInfo.Country := confgFile.ReadString('profile', 'country', DEFAULT_COUNTRY);
  retData.LocationInfo.State := confgFile.ReadString('profile', 'state', DEFAULT_STATE);
  retData.LocationInfo.City := confgFile.ReadString('profile', 'city', DEFAULT_CITY);
  retData.LocationInfo.Lat := confgFile.ReadFloat('profile', 'lat', DEFAULT_LAT);
  retData.LocationInfo.Lng := confgFile.ReadFloat('profile', 'lng', DEFAULT_LNG);

  // Read sensor offset values.
  retData.MagDecOffset := confgFile.ReadFloat('profile', 'magdecoffset', DEFAULT_MAG_DEC);
  retData.InclinOffset := confgFile.ReadFloat('profile', 'incloffset', DEFAULT_INCL_ANGLE);

  // Read date/time related settings.
  retData.UTCOffset := confgFile.ReadString('profile', 'utcoffset', DEFAULT_TIME_ZONE);
  retData.SysTime := confgFile.ReadBool('profile', 'systime', true);

  // Read device path/name.
  retData.DevPath := confgFile.ReadString('profile', 'devpath', DEFAULT_DEV_PATH);

  FreeAndNil(confgFile);
  result := retData;
end;

function TfrmStarPointerSync.UpdateDateTime() : TDateInfo;
var
  retData: TDateInfo;
  tempMs: Word;
  usrTime: TDateTime;
begin
  // Extract date from user inputs.
  retData.Year := StrToIntDef(Trim(txtYear.Text), MIN_YEAR);
  retData.Month := StrToIntDef(Trim(txtMonth.Text), DEFAULT_MONTH);
  retData.Day := StrToIntDef(Trim(txtDate.Text), DEFAULT_DAY);

  // Reset invalid date components.
  if((retData.Year < MIN_YEAR) or (retData.Year > MAX_YEAR)) then
  begin
    retData.Year := MIN_YEAR;
  end;

  if((retData.Month < 1) or (retData.Month > 12)) then
  begin
    retData.Month := DEFAULT_MONTH;
  end;

  if((retData.Day < 1) or (retData.Day > 12)) then
  begin
    retData.Day := DEFAULT_DAY;
  end;

  // Extract time from user inputs.
  retData.Hour := StrToIntDef(Trim(txtHour.Text), DEFAULT_HOUR);
  retData.Minutes := StrToIntDef(Trim(txtMinutes.Text), DEFAULT_MINUTES);
  retData.Seconds := StrToIntDef(Trim(txtSeconds.Text), DEFAULT_SECONDS);

  // Reset invalid time components.
  if((retData.Hour < 0) or (retData.Hour > 59)) then
  begin
    retData.Hour := DEFAULT_HOUR;
  end;

  if((retData.Minutes < 0) or (retData.Minutes > 59)) then
  begin
    retData.Minutes := DEFAULT_MINUTES;
  end;

  if((retData.Seconds < 0) or (retData.Seconds > 59)) then
  begin
    retData.Seconds := DEFAULT_SECONDS;
  end;

  usrTime := GetUTC(retData);

  // Get date/time components of the calculated UTC time.
  DecodeDateTime(usrTime, retData.Year, retData.Month, retData.Day,
    retData.Hour, retData.Minutes, retData.Seconds, tempMs);

  // Update UTC label.
  lblGMTValue.Caption := Format(DATETIME_DISPLAY_FMT,
    [retData.Year, retData.Month, retData.Day, retData.Hour, retData.Minutes,
    retData.Seconds]);

  result := retData;
end;

procedure TfrmStarPointerSync.UpdateSysDateTime();
var
  sysDateTime: TDateInfo;
  tempMs: Word;
begin
  // Get current system date/time.
  DecodeDateTime(Now, sysDateTime.Year, sysDateTime.Month, sysDateTime.Day,
    sysDateTime.Hour, sysDateTime.Minutes, sysDateTime.Seconds, tempMs);

  // Update UI with current system date/time.
  txtYear.Text := IntToStr(sysDateTime.Year);
  txtMonth.Text := IntToStr(sysDateTime.Month);
  txtDate.Text := IntToStr(sysDateTime.Day);
  txtHour.Text := IntToStr(sysDateTime.Hour);
  txtMinutes.Text := IntToStr(sysDateTime.Minutes);
  txtSeconds.Text := IntToStr(sysDateTime.Seconds);

  // Calculate UTC based on current time and update the associated UI elements.
  UpdateDateTime();
end;

end.

