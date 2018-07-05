object frmMain: TfrmMain
  Left = 266
  Top = 96
  Width = 1304
  Height = 806
  Caption = 'TD-STK Debug Monitor V2.1.4'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Arial'
  Font.Style = []
  OldCreateOrder = False
  Position = poDefault
  WindowState = wsMaximized
  OnCloseQuery = FormCloseQuery
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 15
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1296
    Height = 65
    Align = alTop
    BevelInner = bvLowered
    TabOrder = 0
    object Label1: TLabel
      Left = 5
      Top = 8
      Width = 57
      Height = 15
      Caption = 'Serial Port'
    end
    object Label2: TLabel
      Left = 8
      Top = 40
      Width = 59
      Height = 15
      Caption = 'Command'
    end
    object Label3: TLabel
      Left = 239
      Top = 8
      Width = 63
      Height = 15
      Caption = 'Block Head'
    end
    object Label4: TLabel
      Left = 368
      Top = 39
      Width = 28
      Height = 15
      Caption = 'Time'
    end
    object Label5: TLabel
      Left = 512
      Top = 39
      Width = 18
      Height = 15
      Caption = 'ms'
    end
    object ComboBox1: TComboBox
      Left = 66
      Top = 4
      Width = 81
      Height = 23
      Style = csDropDownList
      ItemHeight = 15
      TabOrder = 0
    end
    object Button2: TButton
      Left = 503
      Top = 5
      Width = 85
      Height = 21
      Caption = 'Close'
      Enabled = False
      TabOrder = 2
      OnClick = Button2Click
    end
    object ComboBox2: TComboBox
      Left = 145
      Top = 4
      Width = 81
      Height = 23
      Style = csDropDownList
      ItemHeight = 15
      ItemIndex = 0
      TabOrder = 1
      Text = '115200'
      Items.Strings = (
        '115200'
        '57600'
        '38400'
        '19200'
        '9600'
        '4800')
    end
    object Button1: TButton
      Left = 415
      Top = 5
      Width = 85
      Height = 21
      Caption = 'Open'
      TabOrder = 3
      OnClick = Button1Click
    end
    object ComboBox3: TComboBox
      Left = 165
      Top = 36
      Width = 200
      Height = 23
      Enabled = False
      ItemHeight = 15
      MaxLength = 59
      TabOrder = 4
      Items.Strings = (
        'ccsir,3,1'
        'ccsir,2,1'
        'ccsir,1,1'
        'ccsir,3,0'
        'ccsir,2,0'
        'ccsir,1,0'
        'TDDBG,1'
        'TDCMD,1')
    end
    object Button4: TButton
      Left = 558
      Top = 37
      Width = 85
      Height = 21
      Caption = 'Send'
      Enabled = False
      TabOrder = 5
      OnClick = Button4Click
    end
    object ComboBox4: TComboBox
      Left = 304
      Top = 4
      Width = 110
      Height = 23
      ItemHeight = 15
      MaxLength = 16
      TabOrder = 6
      Items.Strings = (
        'DBG_INFO'
        '$GNGGA'
        '$GPGGA'
        '$BDGGA')
    end
    object Edit1: TEdit
      Left = 400
      Top = 36
      Width = 105
      Height = 23
      TabOrder = 7
      Text = '1000'
    end
    object CheckBox1: TCheckBox
      Left = 536
      Top = 40
      Width = 17
      Height = 17
      TabOrder = 8
      OnClick = CheckBox1Click
    end
    object Button6: TButton
      Left = 592
      Top = 5
      Width = 85
      Height = 21
      Caption = 'Debug'
      Enabled = False
      TabOrder = 9
      OnClick = Button6Click
    end
    object Button7: TButton
      Left = 72
      Top = 37
      Width = 85
      Height = 21
      Caption = 'Setting'
      Enabled = False
      TabOrder = 10
      OnClick = Button7Click
    end
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 65
    Width = 1296
    Height = 688
    ActivePage = TabSheet1
    Align = alClient
    MultiLine = True
    TabIndex = 0
    TabOrder = 1
    object TabSheet1: TTabSheet
      Caption = 'BLOCK'
      object Memo1: TMemo
        Left = 0
        Top = 0
        Width = 1288
        Height = 658
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #40657#20307
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'ROLL'
      ImageIndex = 2
      object Memo2: TMemo
        Left = 0
        Top = 0
        Width = 1288
        Height = 693
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = #40657#20307
        Font.Style = []
        ParentFont = False
        ReadOnly = True
        ScrollBars = ssBoth
        TabOrder = 0
      end
    end
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 753
    Width = 1296
    Height = 19
    Panels = <
      item
        Text = 'Rx CNT:'
        Width = 65
      end
      item
        Text = '0'
        Width = 150
      end
      item
        Text = 'BLK SIZE:'
        Width = 65
      end
      item
        Text = '0'
        Width = 150
      end>
    ParentFont = True
    SimplePanel = False
    UseSystemFont = False
  end
  object Button3: TButton
    Left = 225
    Top = 66
    Width = 120
    Height = 21
    Caption = 'Capture Text'
    Enabled = False
    TabOrder = 3
    OnClick = Button3Click
  end
  object Button5: TButton
    Left = 353
    Top = 66
    Width = 120
    Height = 21
    Caption = 'Clear Memo'
    TabOrder = 4
    OnClick = Button5Click
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 1208
    Top = 16
  end
end
