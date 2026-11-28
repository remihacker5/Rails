// unoffical table of contents
// get libraries

// Declare classes and functions

// in main function:
// get player information
// set everything up for game engine
// create list of players
// create list of objects
// create world

// While game running:
// get updates
// draw players, objects, world. 

#include <cmath>
#include <iostream>
#include <raylib.h>
#include <vector>
#include <algorithm>
using namespace std;

void log(string msg){
    cout << "[GAME LOG] - " + msg << endl;
}

struct WorldRect {
    float left, right, top, bottom;
};

string get_tile(float raw_x, float raw_y, const string& rail_map, const string& world_map, int tile_scale){
    static Texture2D rail = LoadTexture("resources/images/item/rail/rail.png");
    int tile_width = rail.width * tile_scale;
    int tile_height = rail.height * tile_scale;

    int column = static_cast<int>(std::floor(raw_x)) / tile_width - 1;
    int row = static_cast<int>(std::floor(raw_y)) / tile_height;
    if (column < 0 || row < 0){
        return "border";
    }

    auto get_map_tile = [](const string& map, int column, int row) -> char {
        size_t line_start = 0;
        for (int current_row = 0; current_row < row; ++current_row){
            size_t newline = map.find('\n', line_start);
            if (newline == string::npos){
                return '\n';
            }
            line_start = newline + 1;
        }

        size_t index = line_start + static_cast<size_t>(column);
        if (index >= map.length() || map[index] == '\n'){
            return '\n';
        }
        return map[index];
    };

    char rail_tile = get_map_tile(rail_map, column, row);
    if (rail_tile == '|' || rail_tile == '=' || rail_tile == '\\' || rail_tile == '/' || rail_tile == '>' || rail_tile == '<' || rail_tile == 'I'){
        return "rail";
    }

    char world_tile = get_map_tile(world_map, column, row);
    if (rail_tile == '\n' || world_tile == '\n'){
        return "border";
    }
    if (world_tile == '#'){
        return "grass";
    }
    if (world_tile == '%'){
        return "sand";
    }
    if (world_tile == '@'){
        return "rock";
    }
    if (world_tile == ' '){
        return "ocean";
    }
    return "border";
}


string set_tile(float raw_x, float raw_y, string& rail_map, int tile_scale, char new_tile){
    static Texture2D rail = LoadTexture("resources/images/item/rail/rail.png");
    int tile_width = rail.width * tile_scale;
    int tile_height = rail.height * tile_scale;

    int column = static_cast<int>(std::floor(raw_x)) / tile_width - 1;
    int row = static_cast<int>(std::floor(raw_y)) / tile_height;

    auto is_rail_tile = [](char tile) {
        return tile == '|' || tile == '=' || tile == '\\' || tile == '/' ||
               tile == '>' || tile == '<' || tile == 'I';
    };

    auto set_map_tile = [&](string& map, int column, int row, char value, bool only_existing_rail) -> string {
        size_t line_start = 0;
        for (int current_row = 0; current_row < row; ++current_row){
            size_t newline = map.find('\n', line_start);
            line_start = newline + 1;
        }

        size_t index = line_start + static_cast<size_t>(column);
        if (index >= map.length() || map[index] == '\n'){
            return map;
        }
        if (only_existing_rail && !is_rail_tile(map[index])){
            return map;
        }
        if (value == ' '){
            map[index] = value;
            return map;
        }

        char north_tile = map[index - (map.find('\n', 0) + 1)];
        char south_tile = map[index + (map.find('\n', 0) + 1)];
        char east_tile = map[index + 1];
        char west_tile = map[index - 1];
        int tile_count = 0;
        if (north_tile == '=' or north_tile == '|' or north_tile == '\\' or north_tile == '/' or north_tile == '>' or north_tile == '<' or north_tile == 'I'){
            tile_count++;
            north_tile = 'r';
        }
        if (south_tile == '|' or south_tile == '=' or south_tile == '\\' or south_tile == '/' or south_tile == '>' or south_tile == '<' or south_tile == 'I'){
            tile_count++;
            south_tile = 'r';
        }
        if (east_tile == '|' or east_tile == '=' or east_tile == '\\' or east_tile == '/' or east_tile == '>' or east_tile == '<' or east_tile == 'I'){
            tile_count++;
            east_tile = 'r';
        }
        if (west_tile == '|' or west_tile == '=' or west_tile == '\\' or west_tile == '/' or west_tile == '>' or west_tile == '<' or west_tile == 'I'){
            tile_count++;
            west_tile = 'r';
        }


        log("North tile: " + string(1, north_tile));
        log("South tile: " + string(1, south_tile));
        log("East tile: " + string(1, east_tile));
        log("West tile: " + string(1, west_tile));
        log("Tile count: " + std::to_string(tile_count));
        log("--------------------------------");

        if (tile_count == 4){
            value = 'I';
        } else if (tile_count == 3){
            value = 'I';
        } else if (tile_count == 2){
            if ((north_tile == 'r') && (west_tile == 'r')){
                value = '/';
            }else if ((north_tile == 'r') && (east_tile == 'r')){
                value = '\\';
            } else if ((south_tile == 'r') && (west_tile == 'r')){
                value = '>';
            } else if ((south_tile == 'r') && (east_tile == 'r')){
                value = '<';
            } else if ((north_tile == 'r') && (south_tile == 'r')){
                value = '|';
            } else if ((east_tile == 'r') && (west_tile == 'r')){
                value = '=';
            }
        } else if (tile_count == 1){
            if (north_tile == 'r'){
                value = '|';
            } else if (south_tile == 'r'){
                value = '|';
            } else if (east_tile == 'r'){
                value = '=';
            } else if (west_tile == 'r'){
                value = '=';
            }
        }


        map[index] = value;
        return map;
    };

    string new_map = set_map_tile(rail_map, column, row, new_tile, false);
    if (new_tile == ' '){
        return new_map;
    }
    new_map = set_map_tile(rail_map, column, row - 1, new_tile, true);
    new_map = set_map_tile(rail_map, column, row + 1, new_tile, true);
    new_map = set_map_tile(rail_map, column - 1, row, new_tile, true);
    new_map = set_map_tile(rail_map, column + 1, row, new_tile, true);
    return new_map;
}

WorldRect GetCameraWorldBounds(const Camera2D& camera, int screenWidth, int screenHeight) {
    // Handle rotation safely by checking all 4 screen corners
    Vector2 corners[4] = {
        GetScreenToWorld2D({ 0.0f, 0.0f }, camera),
        GetScreenToWorld2D({ (float)screenWidth, 0.0f }, camera),
        GetScreenToWorld2D({ 0.0f, (float)screenHeight }, camera),
        GetScreenToWorld2D({ (float)screenWidth, (float)screenHeight }, camera)
    };

    float minX = corners[0].x, maxX = corners[0].x;
    float minY = corners[0].y, maxY = corners[0].y;

    for (int i = 1; i < 4; ++i) {
        minX = std::min(minX, corners[i].x);
        maxX = std::max(maxX, corners[i].x);
        minY = std::min(minY, corners[i].y);
        maxY = std::max(maxY, corners[i].y);
    }

    return { minX, maxX, minY, maxY };
}

bool IsInView(const WorldRect& view, float x, float y, float width, float height, float margin = 32.0f) {
    return (x + width + margin >= view.left) &&
           (x - margin <= view.right) &&
           (y + height + margin >= view.top) &&
           (y - margin <= view.bottom);
}

vector<size_t> get_row_starts(const string& map) {
    vector<size_t> row_starts{0};
    for (size_t i = 0; i < map.length(); ++i) {
        if (map[i] == '\n' && i + 1 < map.length()) {
            row_starts.push_back(i + 1);
        }
    }
    return row_starts;
}

size_t get_row_end(const string& map, const vector<size_t>& row_starts, size_t row) {
    size_t row_end = row + 1 < row_starts.size() ? row_starts[row + 1] - 1 : map.length();
    if (row_end > row_starts[row] && map[row_end - 1] == '\r') {
        --row_end;
    }
    return row_end;
}

int get_map_width(const string& map, const vector<size_t>& row_starts) {
    size_t width = 0;
    for (size_t row = 0; row < row_starts.size(); ++row) {
        width = max(width, get_row_end(map, row_starts, row) - row_starts[row]);
    }
    return static_cast<int>(width);
}

/*  
   █████████  ████                                            
  ███░░░░░███░░███                                            
 ███     ░░░  ░███   ██████    █████   █████   ██████   █████ 
░███          ░███  ░░░░░███  ███░░   ███░░   ███░░███ ███░░  
░███          ░███   ███████ ░░█████ ░░█████ ░███████ ░░█████ 
░░███     ███ ░███  ███░░███  ░░░░███ ░░░░███░███░░░   ░░░░███
 ░░█████████  █████░░████████ ██████  ██████ ░░██████  ██████ 
  ░░░░░░░░░  ░░░░░  ░░░░░░░░ ░░░░░░  ░░░░░░   ░░░░░░  ░░░░░░  
                                                              
                                                         */

class terrain {
    public:
        Texture2D sand = LoadTexture("resources/images/terrain/sand/sand.png");
        Texture2D rock = LoadTexture("resources/images/terrain/rock/rock.png");
        Texture2D grass1 = LoadTexture("resources/images/terrain/grass/grass1.png");
        Texture2D grass2 = LoadTexture("resources/images/terrain/grass/grass2.png");
        Texture2D grass3 = LoadTexture("resources/images/terrain/grass/grass3.png");
        Texture2D grass4 = LoadTexture("resources/images/terrain/grass/grass4.png");
        int tile_scale = 5;
        string world_map = LoadFileText("resources/world/map.txt");
        vector<size_t> world_row_starts = get_row_starts(world_map);
        int world_map_width = get_map_width(world_map, world_row_starts);
        void draw_grass(float x, float y, int randomgrass){
            if (randomgrass == 1){
                DrawTexturePro(grass1, { 0.0f, 0.0f, (float)grass1.width, (float)grass1.height }, Rectangle { x, y, (float)grass1.width*tile_scale, (float)grass1.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        
            } else if (randomgrass == 2){
                DrawTexturePro(grass2, { 0.0f, 0.0f, (float)grass2.width, (float)grass2.height }, Rectangle { x, y, (float)grass2.width*tile_scale, (float)grass2.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        
            } else if (randomgrass == 3){
                DrawTexturePro(grass3, { 0.0f, 0.0f, (float)grass3.width, (float)grass3.height }, Rectangle { x, y, (float)grass3.width*tile_scale, (float)grass3.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        
            } else if (randomgrass == 4){
                DrawTexturePro(grass4, { 0.0f, 0.0f, (float)grass4.width, (float)grass4.height }, Rectangle { x, y, (float)grass4.width*tile_scale, (float)grass4.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
        }
        void draw_sand(float x, float y){
            DrawTexturePro(sand, { 0.0f, 0.0f, (float)sand.width, (float)sand.height }, Rectangle { x, y, (float)sand.width*tile_scale, (float)sand.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        }
        void draw_rock(float x, float y){
            DrawTexturePro(rock, { 0.0f, 0.0f, (float)rock.width, (float)rock.height }, Rectangle { x, y, (float)rock.width*tile_scale, (float)rock.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        }
        void draw_terrain(WorldRect view, int randomgrass){
            int tile_width = grass1.width * tile_scale;
            int tile_height = grass1.height * tile_scale;
            int first_column = max(0, static_cast<int>(floor((view.left - 32.0f) / tile_width)) - 2);
            int last_column = min(world_map_width, static_cast<int>(ceil((view.right + 32.0f) / tile_width)) + 1);
            int first_row = max(0, static_cast<int>(floor((view.top - 32.0f) / tile_height)) - 1);
            int last_row = min(static_cast<int>(world_row_starts.size()),
                               static_cast<int>(ceil((view.bottom + 32.0f) / tile_height)) + 1);

            for (int row = first_row; row < last_row; ++row) {
                size_t row_start = world_row_starts[row];
                size_t row_end = get_row_end(world_map, world_row_starts, row);
                for (int column = first_column; column < last_column; ++column) {
                    size_t index = row_start + static_cast<size_t>(column);
                    if (index >= row_end) {
                        continue;
                    }

                    float location_x = static_cast<float>((column + 1) * tile_width);
                    float location_y = static_cast<float>(row * tile_height);
                    if (!IsInView(view, location_x, location_y, tile_width, tile_height)) {
                        continue;
                    }

                    int tile_variant = (randomgrass + static_cast<int>(index % 4)) % 4 + 1;
                    if (world_map[index] == '#') {
                        draw_grass(location_x, location_y, tile_variant);
                    } else if (world_map[index] == '%') {
                        draw_sand(location_x, location_y);
                    } else if (world_map[index] == '@') {
                        draw_rock(location_x, location_y);
                    }
                }
            }
        }
};

class railway {
    public:
        Texture2D rail_ns = LoadTexture("resources/images/item/rail/rail_ns.png");
        Texture2D rail_ew = LoadTexture("resources/images/item/rail/rail_ew.png");
        Texture2D rail_ne = LoadTexture("resources/images/item/rail/rail_ne.png");
        Texture2D rail_nw = LoadTexture("resources/images/item/rail/rail_nw.png");
        Texture2D rail_sw = LoadTexture("resources/images/item/rail/rail_sw.png");
        Texture2D rail_se = LoadTexture("resources/images/item/rail/rail_se.png");
        Texture2D rail_inter = LoadTexture("resources/images/item/rail/rail_inter.png");
        int tile_scale = 5;
        string rail_map = LoadFileText("resources/world/rail_map.txt");
        vector<size_t> rail_row_starts = get_row_starts(rail_map);
        int rail_map_width = get_map_width(rail_map, rail_row_starts);
        void draw_rail(float x, float y, char tile){
            
            if (tile == '|'){
                DrawTexturePro(rail_ns, { 0.0f, 0.0f, (float)rail_ns.width, (float)rail_ns.height }, Rectangle { x, y, (float)rail_ns.width*tile_scale, (float)rail_ns.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == '='){
                DrawTexturePro(rail_ew, { 0.0f, 0.0f, (float)rail_ew.width, (float)rail_ew.height }, Rectangle { x, y, (float)rail_ew.width*tile_scale, (float)rail_ew.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == '\\'){
                DrawTexturePro(rail_ne, { 0.0f, 0.0f, (float)rail_ne.width, (float)rail_ne.height }, Rectangle { x, y, (float)rail_ne.width*tile_scale, (float)rail_ne.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == '/'){
                DrawTexturePro(rail_nw, { 0.0f, 0.0f, (float)rail_nw.width, (float)rail_nw.height }, Rectangle { x, y, (float)rail_nw.width*tile_scale, (float)rail_nw.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == '>'){
                DrawTexturePro(rail_sw, { 0.0f, 0.0f, (float)rail_sw.width, (float)rail_sw.height }, Rectangle { x, y, (float)rail_sw.width*tile_scale, (float)rail_sw.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == '<'){
                DrawTexturePro(rail_se, { 0.0f, 0.0f, (float)rail_se.width, (float)rail_se.height }, Rectangle { x, y, (float)rail_se.width*tile_scale, (float)rail_se.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
            if (tile == 'I'){
                DrawTexturePro(rail_inter, { 0.0f, 0.0f, (float)rail_inter.width, (float)rail_inter.height }, Rectangle { x, y, (float)rail_inter.width*tile_scale, (float)rail_inter.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
            }
        }
        void draw_railway(WorldRect view){
            int tile_width = rail_ns.width * tile_scale;
            int tile_height = rail_ns.height * tile_scale;
            int first_column = max(0, static_cast<int>(floor((view.left - 32.0f) / tile_width)) - 2);
            int last_column = min(rail_map_width, static_cast<int>(ceil((view.right + 32.0f) / tile_width)) + 1);
            int first_row = max(0, static_cast<int>(floor((view.top - 32.0f) / tile_height)) - 1);
            int last_row = min(static_cast<int>(rail_row_starts.size()),
                               static_cast<int>(ceil((view.bottom + 32.0f) / tile_height)) + 1);

            for (int row = first_row; row < last_row; ++row) {
                size_t row_start = rail_row_starts[row];
                size_t row_end = get_row_end(rail_map, rail_row_starts, row);
                for (int column = first_column; column < last_column; ++column) {
                    size_t index = row_start + static_cast<size_t>(column);
                    if (index >= row_end) {
                        continue;
                    }

                    char tile = rail_map[index];
                    if (tile != '|' && tile != '=' && tile != '\\' && tile != '/' &&
                        tile != '>' && tile != '<' && tile != 'I') {
                        continue;
                    }

                    float location_x = static_cast<float>((column + 1) * tile_width);
                    float location_y = static_cast<float>(row * tile_height);
                    if (IsInView(view, location_x, location_y, tile_width, tile_height)) {
                        draw_rail(location_x, location_y, tile);
                    }
                }
            }
        }

};

class player {
    private:
        string name = "Kara1";
        string UID = "";
        Texture2D skin = LoadTexture("resources/images/player/Kara/kara-walk.png");
        int anim_frame = 0;
        int facing = -1;
        float location_x = 38050.0f;
        float location_y = 43900.0f;
        int move_speed_x = 3;
        int move_speed_y = 3;
        int player_scale = 5;
        

        // MOVE SPEED GETTER AND SETTER
        int get_move_speed_x () {
            return move_speed_x;
        }
        int get_move_speed_y () {
            return move_speed_y;
        }
         
        void play_walking_animation() {
            
                // Play the walking animation for the player character to the right
                static int frames = 0;
                frames++;

                if (anim_frame == 0 && frames == 10){
                    frames = 0;
                    anim_frame = 1;
                } 
                else if (anim_frame == 1 && frames == 10){
                    frames = 0;
                    anim_frame = 2;
                }
                else if (anim_frame == 2 && frames == 10){
                    frames = 0;
                    anim_frame = 3;
                }
                else if (anim_frame == 3 && frames == 10){
                    frames = 0;
                    anim_frame = 0;
                }
        }

    public:
        bool in_cart = false;
        // NAME GETTER AND SETTER
        string get_name () {
            return name;
        }

        void set_name (string newname) {
            name = newname;
            return;
        }

        // LOCATION GETTER AND SETTER
        float get_location_x () {
            return location_x;
        }
        float get_location_y () {
            return location_y;
        }

        void set_location_x (float new_location_x) {
            location_x = new_location_x;
            return;
        }

        void set_location_y (float new_location_y) {
            location_y = new_location_y;
            return;
        }
    
    // SKIN GETTER AND SETTER
        Texture2D get_skin () {
            return skin;
        }

        void set_skin (Texture2D newskin) {
            skin = newskin;
            return;        
        }

        void walk(int direction_x, int direction_y) {
            
            if (direction_x > 0) {
                // Play the walking animation for the player character to the right
                facing = 1;
                play_walking_animation();
            }
            if (direction_x < 0) {
                // Play the walking animation for the player character to the left
                facing = -1;
                play_walking_animation();
            }
            if (direction_y > 0) {
                // Play the walking animation for the player character downwards
                play_walking_animation();
            }
            if (direction_y < 0) {
                // Play the walking animation for the player character upwards
                play_walking_animation();
            }
            if (direction_x == 0 && direction_y == 0){
                anim_frame = 0;
                return;
            }
            location_x += direction_x * move_speed_x;
            location_y += direction_y * move_speed_y;

        }

    // Draw the player character based on their skin and location
        void draw_player(){
            DrawTexturePro(skin, { (float)skin.width/4*anim_frame, 0.0f, (float)skin.width/4 * facing, (float)skin.height }, Rectangle { location_x, location_y, (float)skin.width/4*player_scale, (float)skin.height*player_scale }, Vector2 {0, 0}, 0, WHITE);
        }
};

class cart {
    public:
        float location_x = 38050.0f;
        float location_y = 43900.0f;
        float momentum_x = 0.0f;
        float momentum_y = 0.0f;
        int speed = 7;
        int tile_scale = 5;
        void draw_cart(){
            static Texture2D cart = LoadTexture("resources/images/item/cart/cart.png");
            DrawTexturePro(cart, { 0.0f, 0.0f, (float)cart.width, (float)cart.height }, Rectangle { location_x, location_y, (float)cart.width*tile_scale, (float)cart.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        }

        void calculate_momentum(player& local_player, cart& local_cart, const string& rail_map, const string& world_map){
            float player_center_x = local_player.get_location_x() + (16.0f * 5.0f / 2.0f);
            float player_center_y = local_player.get_location_y() + (16.0f * 5.0f / 2.0f);
            if (local_player.in_cart){
                local_player.set_location_x(local_cart.location_x);
                local_player.set_location_y(local_cart.location_y);
                if (IsKeyDown(KEY_W) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y - 26, rail_map, world_map, tile_scale) == "rail") {
                    local_cart.location_y -= local_cart.speed;
                    local_cart.momentum_y = -5;
                    local_cart.momentum_x = 0;
                }
                if (IsKeyDown(KEY_S) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y + 64, rail_map, world_map, tile_scale) == "rail") {
                    local_cart.location_y += local_cart.speed;
                    local_cart.momentum_y = 5;
                    local_cart.momentum_x = 0;
                }
                if (IsKeyDown(KEY_A) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x - 45, player_center_y, rail_map, world_map, tile_scale) == "rail") {
                    local_cart.location_x -= local_cart.speed;
                    local_cart.momentum_x = -5;
                    local_cart.momentum_y = 0;
                }
                if (IsKeyDown(KEY_D) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x + 45, player_center_y, rail_map, world_map, tile_scale) == "rail") {
                    local_cart.location_x += local_cart.speed;
                    local_cart.momentum_x = 5;
                    local_cart.momentum_y = 0;
                }
            }

            // calculate cart momentum
            if (!(get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x + 45, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x - 45, player_center_y, rail_map, world_map, tile_scale) == "rail")){
                local_cart.momentum_x = 0;
            }

            if (!(get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y + 64, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y - 26, rail_map, world_map, tile_scale) == "rail")){

                local_cart.momentum_y = 0;
            }

            if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_D)){

            
                if (local_cart.momentum_x > 0){
                    local_cart.momentum_x -= 0.1f;
                    local_cart.location_x += local_cart.momentum_x;
                
                    if (local_cart.momentum_x < 0){
                        local_cart.momentum_x = 0;
                    }
                }
                if (local_cart.momentum_y > 0){
                    local_cart.momentum_y -= 0.1f;
                    local_cart.location_y += local_cart.momentum_y;
                    if (local_cart.momentum_y < 0){
                        local_cart.momentum_y = 0;
                    }
                }
                if (local_cart.momentum_x < 0){
                    local_cart.momentum_x += 0.1f;
                    local_cart.location_x += local_cart.momentum_x;
                    if (local_cart.momentum_x > 0){
                        local_cart.momentum_x = 0;
                    }
                }
                if (local_cart.momentum_y < 0){
                    local_cart.momentum_y += 0.1f;
                    local_cart.location_y += local_cart.momentum_y;
                    if (local_cart.momentum_y > 0){
                    local_cart.momentum_y = 0;
                    }
                }
            }
        }
};

class HUD {
    public:
        int active_slot = 1;
        Texture2D toolbar = LoadTexture("resources/images/menu/toolbar.png");
        Texture2D toolselect = LoadTexture("resources/images/menu/toolselect.png");
        Texture2D rail = LoadTexture("resources/images/item/rail/rail_ns.png");
        Texture2D pickaxe = LoadTexture("resources/images/item/pickaxe/pickaxe.png");
        Texture2D Black_gem = LoadTexture("resources/images/item/gem/black_gem.png");
        Texture2D White_gem = LoadTexture("resources/images/item/gem/white_gem.png");
        Texture2D Blue_gem = LoadTexture("resources/images/item/gem/blue_gem.png");
        Texture2D Red_gem = LoadTexture("resources/images/item/gem/red_gem.png");
        Texture2D Green_gem = LoadTexture("resources/images/item/gem/green_gem.png");
        Texture2D mining_anim = LoadTexture("resources/images/item/pickaxe/mining.png");
        int hud_scale = 5;
        int anim_frame = 1;
        vector<string> inventory = {"pickaxe","rail","","","","",""};
        vector<int> inventory_count = {1,10,0,0,0,0,0};
        void draw_toolbar(HUD local_hud){
            DrawTexturePro(local_hud.toolbar, { 0.0f, 0.0f, (float)local_hud.toolbar.width, (float)local_hud.toolbar.height }, Rectangle { (GetScreenWidth()/2)-((local_hud.toolbar.width*local_hud.hud_scale)/2), GetScreenHeight()-(local_hud.toolbar.height*local_hud.hud_scale) - 5, (float)local_hud.toolbar.width*local_hud.hud_scale, (float)local_hud.toolbar.height*local_hud.hud_scale }, Vector2 {0, 0}, 0, WHITE);
            for (size_t i = 0; i < inventory.size(); ++i){
                if (local_hud.inventory_count[i] < 0){
                    local_hud.inventory_count[i] = 0;
                }
                if (local_hud.inventory_count[i] == 0){
                    local_hud.inventory[i] = "";
                }
                if (local_hud.inventory[i] != ""){
                    if (local_hud.inventory[i] == "rail"){
                        DrawTexturePro(local_hud.rail, { 0.0f, 0.0f, (float)local_hud.rail.width, (float)local_hud.rail.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.rail.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.rail.height*local_hud.hud_scale) + 10, (float)local_hud.rail.width*(local_hud.hud_scale-2), (float)local_hud.rail.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.rail.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.rail.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "pickaxe"){
                        DrawTexturePro(local_hud.pickaxe, { 0.0f, 0.0f, (float)local_hud.pickaxe.width, (float)local_hud.pickaxe.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.pickaxe.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.pickaxe.height*local_hud.hud_scale) + 10, (float)local_hud.pickaxe.width*(local_hud.hud_scale-2), (float)local_hud.pickaxe.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.pickaxe.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.pickaxe.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "Black Gem"){
                        DrawTexturePro(local_hud.Black_gem, { 0.0f, 0.0f, (float)local_hud.Black_gem.width, (float)local_hud.Black_gem.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Black_gem.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.Black_gem.height*local_hud.hud_scale) + 10, (float)local_hud.Black_gem.width*(local_hud.hud_scale-2), (float)local_hud.Black_gem.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Black_gem.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.Black_gem.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "White Gem"){
                        DrawTexturePro(local_hud.White_gem, { 0.0f, 0.0f, (float)local_hud.White_gem.width, (float)local_hud.White_gem.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.White_gem.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.White_gem.height*local_hud.hud_scale) + 10, (float)local_hud.White_gem.width*(local_hud.hud_scale-2), (float)local_hud.White_gem.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.White_gem.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.White_gem.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "Blue Gem"){
                        DrawTexturePro(local_hud.Blue_gem, { 0.0f, 0.0f, (float)local_hud.Blue_gem.width, (float)local_hud.Blue_gem.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Blue_gem.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.Blue_gem.height*local_hud.hud_scale) + 10, (float)local_hud.Blue_gem.width*(local_hud.hud_scale-2), (float)local_hud.Blue_gem.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Blue_gem.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.Blue_gem.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "Red Gem"){
                        DrawTexturePro(local_hud.Red_gem, { 0.0f, 0.0f, (float)local_hud.Red_gem.width, (float)local_hud.Red_gem.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Red_gem.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.Red_gem.height*local_hud.hud_scale) + 10, (float)local_hud.Red_gem.width*(local_hud.hud_scale-2), (float)local_hud.Red_gem.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Red_gem.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.Red_gem.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    if (local_hud.inventory[i] == "Green Gem"){
                        DrawTexturePro(local_hud.Green_gem, { 0.0f, 0.0f, (float)local_hud.Green_gem.width, (float)local_hud.Green_gem.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Green_gem.width*local_hud.hud_scale)*i) + 15, GetScreenHeight()-(local_hud.Green_gem.height*local_hud.hud_scale) + 10, (float)local_hud.Green_gem.width*(local_hud.hud_scale-2), (float)local_hud.Green_gem.height*(local_hud.hud_scale-2)}, Vector2 {0, 0}, 0, WHITE);
                        if (local_hud.inventory_count[i] > 1){
                            DrawText(std::to_string(local_hud.inventory_count[i]).c_str(), (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.Green_gem.width*local_hud.hud_scale)*i) + 10, GetScreenHeight()-(local_hud.Green_gem.height*local_hud.hud_scale) + 2, 30, WHITE);
                        }
                    }
                    
                    
                }
            }
            
            DrawTexturePro(local_hud.toolselect, { 0.0f, 0.0f, (float)local_hud.toolselect.width, (float)local_hud.toolselect.height }, Rectangle { (GetScreenWidth()/2)-(((local_hud.toolbar.width*local_hud.hud_scale)/2)-5)+((local_hud.toolselect.width*local_hud.hud_scale)*local_hud.active_slot)-(local_hud.toolselect.width*local_hud.hud_scale), GetScreenHeight()-(local_hud.toolselect.height*local_hud.hud_scale) - 5, (float)local_hud.toolselect.width*local_hud.hud_scale, (float)local_hud.toolselect.height*local_hud.hud_scale }, Vector2 {0, 0}, 0, WHITE);

        }

        void play_mining_animation(int x,int y) {
            
                // Play the walking animation for the player character to the right
                static int frames = 0;
                frames++;

                if (anim_frame == 0 && frames == 35){
                    frames = 0;
                    anim_frame = 1;
                } 
                else if (anim_frame == 1 && frames == 35){
                    frames = 0;
                    anim_frame = 2;
                }
                else if (anim_frame == 2 && frames == 35){
                    frames = 0;
                    anim_frame = 3;
                }
                else if (anim_frame == 3 && frames == 35){
                    frames = 0;
                    anim_frame = 0;
                }
                DrawTexturePro(mining_anim, { (float)mining_anim.width/4*anim_frame, 0.0f, (float)mining_anim.width/4, (float)mining_anim.height }, Rectangle { x - (float)mining_anim.width/2, y - (float)mining_anim.height*2, (float)mining_anim.width/4*hud_scale, (float)mining_anim.height*hud_scale }, Vector2 {0, 0}, 0, WHITE);
        }

        void mine(HUD& local_hud){
            static int startframe = 0;
            int mine_result = floor(GetRandomValue(1, 80));  
            startframe += 1;
            
            if (120 <= startframe){
                    // Search an element 22
                auto empty_slot = find(local_hud.inventory.begin(), local_hud.inventory.end(), "");
  	                // Check if element is present
              	if (empty_slot != local_hud.inventory.end()){
    	                // Print index
                    int index = distance(local_hud.inventory.begin(), empty_slot);
                    if (mine_result == 1){
                        local_hud.inventory[index] = "Black Gem";
                        local_hud.inventory_count[index] += 1;
                        startframe = 0;
                    }
                    if (mine_result > 1 && mine_result <= 10){
                        local_hud.inventory[index] = "White Gem";
                        local_hud.inventory_count[index] += 1;
                        startframe = 0;
                    }
                    if (mine_result > 10 && mine_result <= 20){
                        local_hud.inventory[index] = "Blue Gem";
                        local_hud.inventory_count[index] += 1;
                        startframe = 0;
                    }
                    if (mine_result > 20 && mine_result <= 30){
                        local_hud.inventory[index] = "Red Gem";
                        local_hud.inventory_count[index] += 1;
                        startframe = 0;
                    }
                    if (mine_result > 30 && mine_result <= 40){
                        local_hud.inventory[index] = "Green Gem";
                        local_hud.inventory_count[index] += 1;
                        startframe = 0;
                    }
                }
                else{
                    cout << "Not enough space in inventory, tell player.";
                }
                
            }
        }
        void set_mouse(HUD local_hud){
            if (local_hud.inventory[active_slot-1] == "rail"){
                HideCursor();
                DrawTexturePro(local_hud.rail, { 0.0f, 0.0f, (float)local_hud.rail.width, (float)local_hud.rail.height }, Rectangle { GetMousePosition().x, GetMousePosition().y, (float)local_hud.toolselect.width, (float)local_hud.toolselect.height}, Vector2 {0, 0}, 0, WHITE);
            }
            else if (local_hud.inventory[active_slot-1] == "pickaxe"){
                HideCursor();
                DrawTexturePro(local_hud.pickaxe, { 0.0f, 0.0f, (float)local_hud.pickaxe.width, (float)local_hud.pickaxe.height }, Rectangle { GetMousePosition().x, GetMousePosition().y, (float)local_hud.toolselect.width, (float)local_hud.toolselect.height}, Vector2 {0, 0}, 0, WHITE);
            }
            else {
                ShowCursor();
            }
        }
        void draw_HUD(HUD local_HUD){
            local_HUD.draw_toolbar(local_HUD); 
            local_HUD.set_mouse(local_HUD);
        }
        

};
/* 
 ██████   ██████            ███            
░░██████ ██████            ░░░             
 ░███░█████░███   ██████   ████  ████████  
 ░███░░███ ░███  ░░░░░███ ░░███ ░░███░░███ 
 ░███ ░░░  ░███   ███████  ░███  ░███ ░███ 
 ░███      ░███  ███░░███  ░███  ░███ ░███ 
 █████     █████░░████████ █████ ████ █████
░░░░░     ░░░░░  ░░░░░░░░ ░░░░░ ░░░░ ░░░░░ 
*/



int main () {

    int SCREEN_WIDTH = 1280;
    int SCREEN_HEIGHT = 720;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    int framesCounter = 0;
    bool debug_mode = true;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rails - PreAlpha 1");
    SetWindowIcon(LoadImage("resources/images/menu/icon.png"));
    SetTargetFPS(60);
    log("Screen init SUCCESS");
    int tile_scale = 5;
    string world_map = LoadFileText("resources/world/map.txt");
    string rail_map = LoadFileText("resources/world/rail_map.txt");
    float mousewheel = GetMouseWheelMove();
    int randomgrass = 1;
    Camera2D camera = { 0 };
    player local_player = player();
    terrain local_terrain = terrain();
    railway local_railway = railway();
    HUD local_hud = HUD();
    cart local_cart = cart();
    camera.target = Vector2{local_player.get_location_x(), local_player.get_location_y()};
    camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    
    while (WindowShouldClose() == false){
        // Refresh the screen size and camera target if the window is resized
        if (IsWindowResized()){
            SCREEN_WIDTH = GetScreenWidth();
            SCREEN_HEIGHT = GetScreenHeight();
            camera.target = Vector2{local_player.get_location_x(), local_player.get_location_y()};
            camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }
        framesCounter++; // Increment the frame counter
        mousewheel = GetMouseWheelMove();
        BeginDrawing(); // Start drawing the frame

        // Clear the background and set up the camera
        BeginMode2D(camera);
        ClearBackground(BLUE);
        WorldRect view = GetCameraWorldBounds(camera, SCREEN_WIDTH, SCREEN_HEIGHT);


        local_terrain.draw_terrain(view, randomgrass);


        local_railway.draw_railway(view);






        local_player.draw_player(); // Draw the local player character
        
        // Handle player movement based on key inputs
        
        if (IsKeyDown(KEY_W)) {
            local_player.walk(0, -1);
        }
        if (IsKeyDown(KEY_A)) {
            local_player.walk(-1, 0);
        }
        if (IsKeyDown(KEY_S)) {
            local_player.walk(0, 1);
        }
        if (IsKeyDown(KEY_D)) {
            local_player.walk(1, 0);
        } 
        if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_D)) {
            local_player.walk(0, 0);
        }
        
        if (IsKeyPressed(KEY_F1)){
            debug_mode = !debug_mode;
        }
        if (IsKeyPressed(KEY_E)){
            if (local_player.get_location_x() >= local_cart.location_x - 128 && local_player.get_location_x() <= local_cart.location_x - 16 + (16.0f * 5.0f) && local_player.get_location_y() >= local_cart.location_y - 128 && local_player.get_location_y() <= local_cart.location_y + (16.0f * 5.0f) + 16){
                local_player.in_cart = !local_player.in_cart;
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            Vector2 mouse_position = GetMousePosition();
            Vector2 world_position = GetScreenToWorld2D(mouse_position, camera);
            if (local_hud.inventory[local_hud.active_slot-1] == "rail" && local_hud.inventory_count[local_hud.active_slot-1] > 0 && get_tile(world_position.x, world_position.y, local_railway.rail_map, world_map, tile_scale) != "rail"){
                local_railway.rail_map = set_tile(world_position.x, world_position.y, local_railway.rail_map, tile_scale, '|');
                local_hud.inventory_count[local_hud.active_slot-1] -= 1;
                if(local_hud.inventory_count[local_hud.active_slot-1] == 0){
                    local_hud.inventory[local_hud.active_slot-1] = "";
                }
            }
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
            Vector2 mouse_position = GetMousePosition();
            Vector2 world_position = GetScreenToWorld2D(mouse_position, camera);
            if (get_tile(world_position.x, world_position.y, local_railway.rail_map, local_terrain.world_map, tile_scale) == "rail" && (local_hud.inventory[local_hud.active_slot-1] == "rail" or local_hud.inventory[local_hud.active_slot-1] == "")){
                local_railway.rail_map = set_tile(world_position.x, world_position.y, local_railway.rail_map, tile_scale, ' ');
                local_hud.inventory_count[local_hud.active_slot-1] += 1;
                local_hud.inventory[local_hud.active_slot-1] = "rail";
            }
            
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 mouse_position = GetMousePosition();
            Vector2 world_position = GetScreenToWorld2D(mouse_position, camera);
            if (local_hud.inventory[local_hud.active_slot-1] == "pickaxe" && get_tile(world_position.x, world_position.y, local_railway.rail_map, world_map, tile_scale) != "rock"){
                local_hud.mine(local_hud);
                local_hud.play_mining_animation(world_position.x, world_position.y);
            }
        }
        if (GetMouseWheelMove() != 0.0f && GetMouseWheelMove() < 0.0f){
            local_hud.active_slot++;
            if (local_hud.active_slot > local_hud.inventory.size()){
                local_hud.active_slot = 1;
            }
        }
        if (GetMouseWheelMove() != 0.0f && GetMouseWheelMove() > 0.0f){
            local_hud.active_slot--;
            if (local_hud.active_slot < 1){
                local_hud.active_slot = local_hud.inventory.size();
            }
        }
        local_cart.draw_cart(); // Draw the cart
        local_cart.calculate_momentum(local_player, local_cart, local_railway.rail_map, world_map); // calculate the cart's momentum based on player input and rail conditions

        camera.target = Vector2{local_player.get_location_x(), local_player.get_location_y()}; // set the camera target to the player's location
        
        EndMode2D();
        if (debug_mode){
            DrawText("DEBUG MODE ENABLED", 20, 20, 10, RED);
            DrawText(std::to_string(local_player.get_location_x()).c_str(), 20, 30, 10, WHITE);
            DrawText(std::to_string(local_player.get_location_y()).c_str(), 20, 40, 10, WHITE);
            DrawText(get_tile(local_player.get_location_x(), local_player.get_location_y(), local_railway.rail_map, world_map, tile_scale).c_str(), 20, 50, 10, WHITE);
        }
        local_hud.draw_HUD(local_hud);
        EndDrawing();
        
    }

    CloseWindow();
}