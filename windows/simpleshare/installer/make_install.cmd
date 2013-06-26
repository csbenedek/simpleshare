

candle simpleshare32.wxs
candle WixUI_NoLic.wxs
light -out simpleshare32.msi -ext WixUIExtension WixUI_NoLic.wixobj simpleshare32.wixobj


candle simpleshare64.wxs

light -out simpleshare64.msi -ext WixUIExtension WixUI_NoLic.wixobj simpleshare64.wixobj
