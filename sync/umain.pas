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
    procedure cmbCitySelect(Sender: TObject);
    procedure cmbCountrySelect(Sender: TObject);
    procedure cmbStateSelect(Sender: TObject);
    procedure cmbUTCOffsetSelect(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormShow(Sender: TObject);
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

    function GetUTCTime() : TDateInfo;

    procedure SaveUserConfig(conf: TUserConfig);
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
begin
  try
     // Loading country list and UTC offset list from files.
     SetCountryList();
     SetUTCOffsetList();
  except
    on E:Exception do
    begin
      MessageDlg(Application.Title, E.Message, TMsgDlgType.mtError, [TMsgDlgBtn.mbOK], 0);
    end;
  end;
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
  tempVal : string;
  tempFloat: Extended;
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



  except
    on E:Exception do
    begin
      MessageDlg(Application.Title, E.Message, TMsgDlgType.mtError, [TMsgDlgBtn.mbOK], 0);
    end;
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
  GetUTCTime();
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

function TfrmStarPointerSync.GetUTCTime() : TDateInfo;
var
  retData: TDateInfo;
  utcOffset: string;
  offsetHour, offsetMinute, tempMs: Word;
  offset, usrTime: TDateTime;
  utcOffsetPlus : boolean;
begin
  // Extract date from user inputs.
  retData.Year := StrToIntDef(Trim(txtYear.Text), 2000);
  retData.Month := StrToIntDef(Trim(txtMonth.Text), 1);
  retData.Day := StrToIntDef(Trim(txtDate.Text), 1);

  // Extract time from user inputs.
  retData.Hour := StrToIntDef(Trim(txtHour.Text), 0);
  retData.Minutes := StrToIntDef(Trim(txtMinutes.Text), 0);
  retData.Seconds := StrToIntDef(Trim(txtSeconds.Text), 0);

  // Get UTC offset selection.
  utcOffset := cmbUTCOffset.Text;

  utcOffsetPlus := (utcOffset[1] = '+');
  offsetHour := StrToInt(Copy(utcOffset, 2, 2));
  offsetMinute := StrToInt(Copy(utcOffset, 5, 2));
  offset := EncodeTime(offsetHour, offsetMinute, 0, 0);

  // User specifed time.
  usrTime := EncodeDateTime(retData.Year, retData.Month, retData.Day,
    retData.Hour, retData.Minutes, retData.Seconds, 0);

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

  // Get date/time components of the calculated UTC time.
  DecodeDateTime(usrTime, retData.Year, retData.Month, retData.Day,
    retData.Hour, retData.Minutes, retData.Seconds, tempMs);

  // Update UTC label.
  lblGMTValue.Caption := Format(DATETIME_DISPLAY_FMT,
    [retData.Year, retData.Month, retData.Day, retData.Hour, retData.Minutes,
    retData.Seconds]);;

  result := retData;
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
  confgFile.WriteString('profile', 'incloffset', conf.UTCOffset);
  confgFile.WriteBool('profile', 'systime', conf.SysTime);

  // Write device path/name.
  confgFile.WriteString('profile', 'devpath', conf.DevPath);

  confgFile.UpdateFile;
  FreeAndNil(confgFile);
end;

end.

