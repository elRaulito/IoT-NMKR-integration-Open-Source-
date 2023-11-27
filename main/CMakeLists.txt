# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" connect.c fetch.c server.c camera.c tasks.c)
set(COMPONENT_ADD_INCLUDEDIRS "")


set(COMPONENT_EMBED_TXTFILES
    "html/index.html"
    "html/gateway.html"
    "html/wifi.html"
    "html/apikey.html"
    "html/deviceid.html"
    "html/detection.html"
    "html/boot/js/bootstrap.min.js"
    "html/boot/js/@tensorflow-models/coco.js"
    "html/boot/js/@tensorflow/tfjs.js"
    "html/boot/css/bootstrap.min.css"
    "html/boot/jquery/jquery.min.js"
    
    "html/boot/js/model.json"


)

set(COMPONENT_EMBED_FILES
    "html/img/IOTMKR.png"
)

register_component()
