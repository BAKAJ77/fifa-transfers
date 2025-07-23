set(FTFS_ROOT_SOURCE "${PROJECT_SOURCE_DIR}/src/main.cpp" "${PROJECT_SOURCE_DIR}/src/resource.h" "${PROJECT_SOURCE_DIR}/src/resource.rc")
set(FTFS_EXTERNAL_SOURCE "${PROJECT_SOURCE_DIR}/src/external/glad.c" "${PROJECT_SOURCE_DIR}/src/external/stb_image.cpp")

set(FTFS_CORE_SOURCE "${PROJECT_SOURCE_DIR}/src/core/application_core.h" "${PROJECT_SOURCE_DIR}/src/core/application_core.cpp" 
    "${PROJECT_SOURCE_DIR}/src/core/application_state.h" "${PROJECT_SOURCE_DIR}/src/core/application_state.cpp" "${PROJECT_SOURCE_DIR}/src/core/audio_system.h"
    "${PROJECT_SOURCE_DIR}/src/core/audio_system.cpp"  "${PROJECT_SOURCE_DIR}/src/core/input_system.h" "${PROJECT_SOURCE_DIR}/src/core/input_system.cpp"
    "${PROJECT_SOURCE_DIR}/src/core/window_frame.h" "${PROJECT_SOURCE_DIR}/src/core/window_frame.cpp")

set(FTFS_GRAPHICS_SOURCE "${PROJECT_SOURCE_DIR}/src/graphics/buffer_objects.h" "${PROJECT_SOURCE_DIR}/src/graphics/buffer_objects.cpp"
    "${PROJECT_SOURCE_DIR}/src/graphics/font_loader.h" "${PROJECT_SOURCE_DIR}/src/graphics/font_loader.cpp" "${PROJECT_SOURCE_DIR}/src/graphics/orthogonal_camera.h" 
    "${PROJECT_SOURCE_DIR}/src/graphics/orthogonal_camera.cpp" "${PROJECT_SOURCE_DIR}/src/graphics/renderer.h" "${PROJECT_SOURCE_DIR}/src/graphics/renderer.cpp" 
    "${PROJECT_SOURCE_DIR}/src/graphics/shader_program.h" "${PROJECT_SOURCE_DIR}/src/graphics/shader_program.cpp" "${PROJECT_SOURCE_DIR}/src/graphics/texture_loader.h" 
    "${PROJECT_SOURCE_DIR}/src/graphics/texture_loader.cpp" "${PROJECT_SOURCE_DIR}/src/graphics/vertex_array.h" "${PROJECT_SOURCE_DIR}/src/graphics/vertex_array.cpp")

set(FTFS_INTERFACE_SOURCE "${PROJECT_SOURCE_DIR}/src/interface/button_base.h" "${PROJECT_SOURCE_DIR}/src/interface/button_base.cpp"
    "${PROJECT_SOURCE_DIR}/src/interface/drop_down.h" "${PROJECT_SOURCE_DIR}/src/interface/drop_down.cpp" "${PROJECT_SOURCE_DIR}/src/interface/menu_button.h"
    "${PROJECT_SOURCE_DIR}/src/interface/menu_button.cpp" "${PROJECT_SOURCE_DIR}/src/interface/radio_buttons.h" "${PROJECT_SOURCE_DIR}/src/interface/radio_buttons.cpp" 
    "${PROJECT_SOURCE_DIR}/src/interface/selection_list.h" "${PROJECT_SOURCE_DIR}/src/interface/selection_list.cpp" "${PROJECT_SOURCE_DIR}/src/interface/text_input_field.h"
    "${PROJECT_SOURCE_DIR}/src/interface/text_input_field.cpp" "${PROJECT_SOURCE_DIR}/src/interface/tick_box.h" "${PROJECT_SOURCE_DIR}/src/interface/tick_box.cpp" 
    "${PROJECT_SOURCE_DIR}/src/interface/user_interface.h" "${PROJECT_SOURCE_DIR}/src/interface/user_interface.cpp")

set(FTFS_SERIALIZATION_SOURCE "${PROJECT_SOURCE_DIR}/src/serialization/club_entity.h" "${PROJECT_SOURCE_DIR}/src/serialization/club_entity.cpp"
    "${PROJECT_SOURCE_DIR}/src/serialization/cup_group.h" "${PROJECT_SOURCE_DIR}/src/serialization/cup_group.cpp" "${PROJECT_SOURCE_DIR}/src/serialization/json_loader.h"
    "${PROJECT_SOURCE_DIR}/src/serialization/json_loader.cpp" "${PROJECT_SOURCE_DIR}/src/serialization/league_group.h" "${PROJECT_SOURCE_DIR}/src/serialization/league_group.cpp"
    "${PROJECT_SOURCE_DIR}/src/serialization/player_entity.h" "${PROJECT_SOURCE_DIR}/src/serialization/player_entity.cpp" "${PROJECT_SOURCE_DIR}/src/serialization/save_data.h"
    "${PROJECT_SOURCE_DIR}/src/serialization/save_data.cpp" "${PROJECT_SOURCE_DIR}/src/serialization/user_profile.h" "${PROJECT_SOURCE_DIR}/src/serialization/user_profile.cpp")

set(FTFS_STATES_SOURCE "${PROJECT_SOURCE_DIR}/src/states/continue_game.h" "${PROJECT_SOURCE_DIR}/src/states/continue_game.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/contract_negotiation.h" "${PROJECT_SOURCE_DIR}/src/states/contract_negotiation.cpp" "${PROJECT_SOURCE_DIR}/src/states/contract_response.h"
    "${PROJECT_SOURCE_DIR}/src/states/contract_response.cpp" "${PROJECT_SOURCE_DIR}/src/states/end_competition.h" "${PROJECT_SOURCE_DIR}/src/states/end_competition.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/end_season.h" "${PROJECT_SOURCE_DIR}/src/states/end_season.cpp" "${PROJECT_SOURCE_DIR}/src/states/financials_generation.h"
    "${PROJECT_SOURCE_DIR}/src/states/financials_generation.cpp" "${PROJECT_SOURCE_DIR}/src/states/inbox_interface.h" "${PROJECT_SOURCE_DIR}/src/states/inbox_interface.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/inbox_menu.h" "${PROJECT_SOURCE_DIR}/src/states/inbox_menu.cpp" "${PROJECT_SOURCE_DIR}/src/states/load_save.h"
    "${PROJECT_SOURCE_DIR}/src/states/load_save.cpp" "${PROJECT_SOURCE_DIR}/src/states/main_game.h" "${PROJECT_SOURCE_DIR}/src/states/main_game.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/main_menu.h" "${PROJECT_SOURCE_DIR}/src/states/main_menu.cpp" "${PROJECT_SOURCE_DIR}/src/states/manage_squad.h"
    "${PROJECT_SOURCE_DIR}/src/states/manage_squad.cpp" "${PROJECT_SOURCE_DIR}/src/states/my_club_section.h" "${PROJECT_SOURCE_DIR}/src/states/my_club_section.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/new_save.h" "${PROJECT_SOURCE_DIR}/src/states/new_save.cpp" "${PROJECT_SOURCE_DIR}/src/states/new_season_setup.h"
    "${PROJECT_SOURCE_DIR}/src/states/new_season_setup.cpp" "${PROJECT_SOURCE_DIR}/src/states/objectives.h" "${PROJECT_SOURCE_DIR}/src/states/objectives.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/play_menu.h" "${PROJECT_SOURCE_DIR}/src/states/play_menu.cpp" "${PROJECT_SOURCE_DIR}/src/states/player_growth_generation.h"
    "${PROJECT_SOURCE_DIR}/src/states/player_growth_generation.cpp" "${PROJECT_SOURCE_DIR}/src/states/record_competition.h" "${PROJECT_SOURCE_DIR}/src/states/record_competition.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/release_clause_activation.h" "${PROJECT_SOURCE_DIR}/src/states/release_clause_activation.cpp" "${PROJECT_SOURCE_DIR}/src/states/resource_loader.h"
    "${PROJECT_SOURCE_DIR}/src/states/resource_loader.cpp" "${PROJECT_SOURCE_DIR}/src/states/save_loading.h" "${PROJECT_SOURCE_DIR}/src/states/save_loading.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/save_writing.h" "${PROJECT_SOURCE_DIR}/src/states/save_writing.cpp" "${PROJECT_SOURCE_DIR}/src/states/search_players.h"
    "${PROJECT_SOURCE_DIR}/src/states/search_players.cpp" "${PROJECT_SOURCE_DIR}/src/states/settings.h" "${PROJECT_SOURCE_DIR}/src/states/settings.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/splash_screen.h" "${PROJECT_SOURCE_DIR}/src/states/splash_screen.cpp" "${PROJECT_SOURCE_DIR}/src/states/statistics.h"
    "${PROJECT_SOURCE_DIR}/src/states/statistics.cpp" "${PROJECT_SOURCE_DIR}/src/states/switch_user.h" "${PROJECT_SOURCE_DIR}/src/states/switch_user.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/training_menu.h" "${PROJECT_SOURCE_DIR}/src/states/training_menu.cpp" "${PROJECT_SOURCE_DIR}/src/states/transfer_history.h"
    "${PROJECT_SOURCE_DIR}/src/states/transfer_history.cpp" "${PROJECT_SOURCE_DIR}/src/states/transfer_hub.h" "${PROJECT_SOURCE_DIR}/src/states/transfer_hub.cpp"
    "${PROJECT_SOURCE_DIR}/src/states/transfer_negotiation.h" "${PROJECT_SOURCE_DIR}/src/states/transfer_negotiation.cpp" "${PROJECT_SOURCE_DIR}/src/states/user_setup.h"
    "${PROJECT_SOURCE_DIR}/src/states/user_setup.cpp" "${PROJECT_SOURCE_DIR}/src/states/view_player.h" "${PROJECT_SOURCE_DIR}/src/states/view_player.cpp")

set(FTFS_UTIL_SOURCE "${PROJECT_SOURCE_DIR}/src/util/data_manip.h" "${PROJECT_SOURCE_DIR}/src/util/data_manip.cpp"
    "${PROJECT_SOURCE_DIR}/src/util/directory_system.h" "${PROJECT_SOURCE_DIR}/src/util/directory_system.cpp" "${PROJECT_SOURCE_DIR}/src/util/globals.h"
    "${PROJECT_SOURCE_DIR}/src/util/logging_system.h" "${PROJECT_SOURCE_DIR}/src/util/logging_system.cpp" "${PROJECT_SOURCE_DIR}/src/util/opengl_error.h"
    "${PROJECT_SOURCE_DIR}/src/util/opengl_error.cpp" "${PROJECT_SOURCE_DIR}/src/util/random_engine.h" "${PROJECT_SOURCE_DIR}/src/util/random_engine.cpp"
    "${PROJECT_SOURCE_DIR}/src/util/random_engine.tpp" "${PROJECT_SOURCE_DIR}/src/util/timestamp.h" "${PROJECT_SOURCE_DIR}/src/util/timestamp.cpp")