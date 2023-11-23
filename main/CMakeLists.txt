# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" connect.c server.c sensor.c fetch.c)
set(COMPONENT_ADD_INCLUDEDIRS "")

set(COMPONENT_EMBED_TXTFILES
    "html/index.html"
    "html/scelta.html"
    "html/settings.html"
    "html/wifi.html"
    "html/number.html"
    "html/boot/js/bootstrap.min.js"
    "html/boot/css/bootstrap.min.css"
    "html/boot/jquery/jquery.min.js"

)

set(COMPONENT_EMBED_FILES
    "html/img/pianta.gif"
)

register_component()
