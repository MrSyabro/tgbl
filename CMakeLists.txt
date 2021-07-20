idf_component_register(SRCS "tgbl.c" "tgbl_parser.c" "tgbl_request.c"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES root_cert.pem
                       REQUIRES jsmn esp-tls esp_http_client)