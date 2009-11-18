#!/bin/sh

valac -H src/midgard_key_config_common.h -C -b vala -d src --target-glib=2.14 --library=midgard vala/midgard_key_config*.vala
