valac --debug --use-header core/MidgardCore.vapi  --vapidir=core -H public_headers/midgard_config.h -C vala/midgard_config.vala
gcc --shared `pkg-config --cflags --libs glib-2.0` -I./core -I./public_headers vala/midgard_config.c core/midgard_core_config.c
