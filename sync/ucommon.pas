unit ucommon;

{$mode ObjFPC}{$H+}

interface

uses
  Classes, SysUtils;

type
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
  CONFIG_FILE_NAME : string = 'starpointersync.ini';
  DATETIME_DISPLAY_FMT : string = '%.4d / %.2d / %.2d - %.2d : %.2d : %.2d';

resourcestring
  ERR_COUNTRY_NOT_DEFINED = 'Country is not selected';
  ERR_STATE_NOT_DEFINED = 'State is not selected';
  ERR_CITY_NOT_DEFINED = 'City is not selected';
  ERR_LAT_NOT_DEFINED = 'Latitude is not specified';
  ERR_INVALID_LAT = 'Invalid or unsupported latitude value';
  ERR_LNG_NOT_DEFINED = 'Longitude is not specified';
  ERR_INVALID_LNG = 'Invalid or unsupported longitude value';

implementation

end.

