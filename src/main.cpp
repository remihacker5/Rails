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
        void draw_terrain(terrain local_terrain, WorldRect view, int randomgrass){
                    // Draw the world map
            float location_x = 0.0f;
            float location_y = 0.0f;
            for (size_t i = 0; i < world_map.length(); ++i) {
                randomgrass++;
                if (randomgrass > 4){
                    randomgrass = 1;
                }
                if (world_map[i] == '#'){
                location_x += grass1.width * tile_scale;
                if (IsInView(view, location_x, location_y, grass1.width * tile_scale, grass1.height * tile_scale, 32.0f)) {
                    local_terrain.draw_grass(location_x, location_y, randomgrass);
                }
                
            }
            if (world_map[i] == '%'){
                location_x += sand.width * tile_scale;
                if (IsInView(view, location_x, location_y, sand.width * tile_scale, sand.height * tile_scale, 32.0f)) {
                    local_terrain.draw_sand(location_x, location_y);
                }
            }
            if (world_map[i] == '@'){
                location_x += rock.width * tile_scale;
                if (IsInView(view, location_x, location_y, rock.width * tile_scale, rock.height * tile_scale, 32.0f)) {
                    local_terrain.draw_rock(location_x, location_y);
                }
            }
            if (world_map[i] == '\n'){
            location_x = 0.0f;
            location_y += grass1.height * tile_scale;
            }
            if (world_map[i] == ' '){
            location_x += grass1.width * tile_scale;
            }            
        }
        randomgrass = 1;
        }
};

class railway {
    public:
        void draw_rail(float x, float y, char tile){
            static Texture2D rail_ns = LoadTexture("resources/images/item/rail/rail_ns.png");
            static Texture2D rail_ew = LoadTexture("resources/images/item/rail/rail_ew.png");
            static Texture2D rail_ne = LoadTexture("resources/images/item/rail/rail_ne.png");
            static Texture2D rail_nw = LoadTexture("resources/images/item/rail/rail_nw.png");
            static Texture2D rail_sw = LoadTexture("resources/images/item/rail/rail_sw.png");
            static Texture2D rail_se = LoadTexture("resources/images/item/rail/rail_se.png");
            static Texture2D rail_inter = LoadTexture("resources/images/item/rail/rail_inter.png");
            static int tile_scale = 5;
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
};

class cart {
    public:
        float location_x = 7280.0f;
        float location_y = 8552.0f;
        float momentum_x = 0.0f;
        float momentum_y = 0.0f;
        void draw_cart(){
            static Texture2D cart = LoadTexture("resources/images/item/cart/cart.png");
            static int tile_scale = 5;
            DrawTexturePro(cart, { 0.0f, 0.0f, (float)cart.width, (float)cart.height }, Rectangle { location_x, location_y, (float)cart.width*tile_scale, (float)cart.height*tile_scale }, Vector2 {0, 0}, 0, WHITE);
        }
};

class player {
    private:
        string name = "Kara1";
        string UID = "";
        Texture2D skin = LoadTexture("resources/images/player/Kara/kara-walk.png");
        int anim_frame = 0;
        int facing = -1;
        float location_x = 7900.0f;
        float location_y = 8600.0f;
        int move_speed_x = 2;
        int move_speed_y = 2;
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

    Texture2D rail = LoadTexture("resources/images/item/rail/rail.png");
    int tile_scale = 5;
    string world_map = LoadFileText("resources/world/map.txt");
    string rail_map = LoadFileText("resources/world/rail_map.txt");
    int randomgrass = 1;
    Camera2D camera = { 0 };
    player local_player = player();
    terrain local_terrain = terrain();
    railway local_railway = railway();
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
        BeginDrawing(); // Start drawing the frame

        // Clear the background and set up the camera
        BeginMode2D(camera);
        ClearBackground(BLUE);
        WorldRect view = GetCameraWorldBounds(camera, SCREEN_WIDTH, SCREEN_HEIGHT);


        local_terrain.draw_terrain(local_terrain, view, randomgrass);


        // Draw the rail map
        float location_x = 0.0f;
        float location_y = 0.0f;
        for (size_t i = 0; i < rail_map.length(); ++i) {
            
            // TODO: add check for invalid chars
            if (rail_map[i] == '|' or rail_map[i] == '=' or rail_map[i] == '\\' or rail_map[i] == '/' or rail_map[i] == '>' or rail_map[i] == '<' or rail_map[i] == 'I'){
                location_x += rail.width * tile_scale;
                if (IsInView(view, location_x, location_y, rail.width * tile_scale, rail.height * tile_scale, 32.0f)) {
                    local_railway.draw_rail(location_x, location_y, rail_map[i]);
                }
            }
            if (rail_map[i] == '\n'){
            location_x = 0.0f;
            location_y += rail.height * tile_scale;
            }
            if (rail_map[i] == ' '){
            location_x += rail.width * tile_scale;
            }            
        }






        local_player.draw_player(); // Draw the local player character
        local_cart.draw_cart(); // Draw the cart
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
        } if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_D)) {
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
        
        float player_center_x = local_player.get_location_x() + (16.0f * 5.0f / 2.0f);
        float player_center_y = local_player.get_location_y() + (16.0f * 5.0f / 2.0f);
        if (local_player.in_cart){
            local_player.set_location_x(local_cart.location_x);
            local_player.set_location_y(local_cart.location_y);
            if (IsKeyDown(KEY_W) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y - 26, rail_map, world_map, tile_scale) == "rail") {
                local_cart.location_y -= 5;
                local_cart.momentum_y = -5;
                local_cart.momentum_x = 0;
            }
            if (IsKeyDown(KEY_S) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x, player_center_y + 64, rail_map, world_map, tile_scale) == "rail") {
                local_cart.location_y += 5;
                local_cart.momentum_y = 5;
                local_cart.momentum_x = 0;
            }
            if (IsKeyDown(KEY_A) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x - 45, player_center_y, rail_map, world_map, tile_scale) == "rail") {
                local_cart.location_x -= 5;
                local_cart.momentum_x = -5;
                local_cart.momentum_y = 0;
            }
            if (IsKeyDown(KEY_D) and get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale) == "rail" and get_tile(player_center_x + 45, player_center_y, rail_map, world_map, tile_scale) == "rail") {
                local_cart.location_x += 5;
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

        camera.target = Vector2{local_player.get_location_x(), local_player.get_location_y()}; // set the camera target to the player's location
        EndMode2D();
        
        if (debug_mode){
            DrawText("DEBUG MODE ENABLED", 20, 20, 10, RED);
            DrawText(std::to_string(local_player.get_location_x()).c_str(), 20, 30, 10, WHITE);
            DrawText(std::to_string(local_player.get_location_y()).c_str(), 20, 40, 10, WHITE);
            DrawText(get_tile(player_center_x, player_center_y, rail_map, world_map, tile_scale).c_str(), 20, 50, 10, WHITE);
        }
         
        EndDrawing();
    }

    CloseWindow();
}