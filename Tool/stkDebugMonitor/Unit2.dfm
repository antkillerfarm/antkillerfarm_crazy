object Form2: TForm2
  Left = 699
  Top = 183
  Width = 399
  Height = 417
  Caption = 'Setting(Only for TD1020)'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label0: TLabel
    Left = 24
    Top = 24
    Width = 145
    Height = 13
    AutoSize = False
    Caption = 'Command'
  end
  object Label1: TLabel
    Left = 24
    Top = 56
    Width = 145
    Height = 13
    AutoSize = False
    Caption = 'Para 1'
    Visible = False
  end
  object Label2: TLabel
    Left = 24
    Top = 88
    Width = 145
    Height = 13
    AutoSize = False
    Caption = 'Para 2'
    Visible = False
  end
  object Label3: TLabel
    Left = 24
    Top = 120
    Width = 145
    Height = 13
    AutoSize = False
    Caption = 'Para 3'
    Visible = False
  end
  object Button1: TButton
    Left = 24
    Top = 336
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = Button1Click
  end
  object ComboBox0: TComboBox
    Left = 173
    Top = 20
    Width = 200
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    MaxLength = 59
    TabOrder = 1
    OnSelect = ComboBox0Select
  end
  object ComboBox1: TComboBox
    Left = 173
    Top = 52
    Width = 200
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    MaxLength = 59
    TabOrder = 2
    Visible = False
    OnSelect = ComboBox1Select
  end
  object ComboBox2: TComboBox
    Left = 173
    Top = 84
    Width = 200
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    MaxLength = 59
    TabOrder = 3
    Visible = False
    OnSelect = ComboBox2Select
  end
  object ComboBox3: TComboBox
    Left = 173
    Top = 116
    Width = 200
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    MaxLength = 59
    TabOrder = 4
    Visible = False
    OnSelect = ComboBox3Select
  end
end
