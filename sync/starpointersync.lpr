program starpointersync;

{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}
  cthreads,
  {$ENDIF}
  {$IFDEF HASAMIGA}
  athreads,
  {$ENDIF}
  Interfaces, // this includes the LCL widgetset
  Forms, umain, ucommon
  { you can add units after this };

{$R *.res}

begin
  RequireDerivedFormResource:=True;
  Application.Title:='Star Pointer Sync';
  Application.Scaled:=True;
  Application.Initialize;
  Application.CreateForm(TfrmStarPointerSync, frmStarPointerSync);
  Application.Run;
end.

