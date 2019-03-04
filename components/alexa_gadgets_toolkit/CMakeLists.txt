set(COMPONENT_SRCDIRS "src" "pb")
set(COMPONENT_ADD_INCLUDEDIRS "src" "pb")
register_component()
component_compile_options("-DPB_FIELD_16BIT")
