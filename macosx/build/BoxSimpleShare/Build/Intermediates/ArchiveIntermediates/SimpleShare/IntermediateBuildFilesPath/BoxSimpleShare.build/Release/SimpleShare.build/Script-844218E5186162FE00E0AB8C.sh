#!/bin/sh
LOCATION="${BUILT_PRODUCTS_DIR}"/"${FRAMEWORKS_FOLDER_PATH}"
CONTENTS="${BUILT_PRODUCTS_DIR}"/"${CONTENTS_FOLDER_PATH}"
#IDENTITY="Mac Developer: Aaron Levie (8TCQ7LXTCG)"

IDENTITY="3rd Party Mac Developer Application: Aaron Levie (956ZZDEPUK)"

codesign --verbose --force --sign "$IDENTITY" "$LOCATION/Growl.framework"
codesign --verbose --force --sign "$IDENTITY" "$LOCATION/GTL.framework"
#codesign --verbose --force --sign "$IDENTITY" "$CONTENTS/Library/LoginItems/LoginHelper.app/"
