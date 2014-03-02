#!/bin/sh
LOCATION="${BUILT_PRODUCTS_DIR}"/"${FRAMEWORKS_FOLDER_PATH}"
CONTENTS="${BUILT_PRODUCTS_DIR}"/"${CONTENTS_FOLDER_PATH}"
IDENTITY="3rd Party Mac Developer Application: Aaron Levie"
codesign --verbose --force --sign "$IDENTITY" "$LOCATION/Growl.framework/"
