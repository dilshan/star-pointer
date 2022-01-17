unit ucommon;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils;

type
  TComState = (CSIdle, CSDate, CSTime, CSLat, CSLng, CSMagOffset, CSInclOffset);

  PCountyInfo=^TCountyInfo;
  TCountyInfo = record
    Lat : Extended;
    Lng : Extended;
    Country : string;
    City : string;
    State: string;
  end;

  TDateInfo = record
    Day: Word;
    Month: Word;
    Year: Word;
    Hour: Word;
    Minutes: Word;
    Seconds: Word;
  end;

  TUserConfig = record
    LocationInfo: TCountyInfo;
    MagDecOffset: Extended;
    InclinOffset: Extended;
    UTCOffset: string;
    SysTime: Boolean;
    DevPath: string;
  end;

const
  MIN_YEAR : Word = 2000;
  MAX_YEAR : Word = 2038;

  CONFIG_FILE_NAME : string = 'starpointersync.ini';
  DATETIME_DISPLAY_FMT : string = '%.4d / %.2d / %.2d - %.2d : %.2d : %.2d';

  DEFAULT_COUNTRY : string = 'Sri Lanka';
  DEFAULT_STATE : string = 'Western';
  DEFAULT_CITY : string = 'Sri Jayewardenepura Kotte';
  DEFAULT_LAT : Extended = 6.9000;
  DEFAULT_LNG : Extended = 79.9164;
  DEFAULT_DEV_PATH : string = '';
  DEFAULT_MAG_DEC : Extended = -2.8;
  DEFAULT_INCL_ANGLE : Extended = 0;
  DEFAULT_TIME_ZONE : string = '+05:30';

  DEFAULT_YEAR : Word = 2000;
  DEFAULT_MONTH : Word = 1;
  DEFAULT_DAY : Word = 1;
  DEFAULT_HOUR : Word = 0;
  DEFAULT_MINUTES : Word = 0;
  DEFAULT_SECONDS : Word = 0;

resourcestring
  ERR_COUNTRY_NOT_DEFINED = 'Country is not selected';
  ERR_STATE_NOT_DEFINED = 'State is not selected';
  ERR_CITY_NOT_DEFINED = 'City is not selected';
  ERR_LAT_NOT_DEFINED = 'Latitude is not specified';
  ERR_INVALID_LAT = 'Invalid or unsupported latitude value';
  ERR_LNG_NOT_DEFINED = 'Longitude is not specified';
  ERR_INVALID_LNG = 'Invalid or unsupported longitude value';

  ERR_MAG_DEC_NOT_DEFINED = 'Magnetic declination offset is not specified';
  ERR_INVALID_MAG_DEC = 'Invalid magnetic declination offset value';
  ERR_INCL_OFFSET_NOT_DEFINED = 'Inclination offset is not specified';
  ERR_INVALID_INCL_OFFSET = 'Invalid inclination offset value';

  ERR_UTC_OFFSET_NOT_DEFINED = 'UTC offset is not selected';
  ERR_DEV_PATH_NOT_DEFINDED = 'Device path/name is not specified';

  ERR_YEAR_NOT_DEFINED = 'Year is not specified';
  ERR_INVALID_YEAR = 'Invalid year value';
  ERR_MONTH_NOT_DEFINED = 'Month is not specified';
  ERR_INVALID_MONTH = 'Invalid month value';
  ERR_DAY_NOT_DEFINED = 'Day is not specified';
  ERR_INVALID_DAY = 'Invalid day value';

  ERR_HOUR_NOT_DEFINED = 'Hour is not specified';
  ERR_INVALID_HOUR = 'Invalid hour value';
  ERR_MINUTE_NOT_DEFINED = 'Minute is not specified';
  ERR_INVALID_MINUTE = 'Invalid minute value';
  ERR_SECONDS_NOT_DEFINED = 'Seconds is not specified';
  ERR_INVALID_SECONDS = 'Invalid seconds value';

implementation

end.

