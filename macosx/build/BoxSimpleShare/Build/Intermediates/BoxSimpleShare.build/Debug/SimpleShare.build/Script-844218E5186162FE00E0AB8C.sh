#!/bin/sh
LOCATION="${BUILT_PRODUCTS_DIR}"/"${FRAMEWORKS_FOLDER_PATH}"
CONTENTS="${BUILT_PRODUCTS_DIR}"/"${CONTENTS_FOLDER_PATH}"
IDENTITY="Unplugged"
codesign --verbose --force --sign "$IDENTITY" "$LOCATION/Growl.framework/"
codesign --verbose --force --sign "$IDENTITY" "$LOCATION/GTL.framework/"
