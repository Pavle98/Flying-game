#include <main_state.h>
#include <glad/glad.h>
#include <math.h>


#include <rafgl.h>

#include <game_constants.h>



static rafgl_raster_t raster;

static rafgl_texture_t texture;
static rafgl_spritesheet_t airplane;

#define NUMBER_OF_TILES 17
rafgl_raster_t tiles[NUMBER_OF_TILES];


#define WORLD_SIZE 128
int tile_world[WORLD_SIZE][WORLD_SIZE];

#define TILE_SIZE 64

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];

int camx = 600, camy = 600;
int selected_x, selected_y;


int frameMatrix[5][4];




void init_tilemap(void)
{
    int x, y;
    rafgl_spritesheet_init(&airplane, "res/images/airplane.png", 5, 5);
    for(y = 0; y < WORLD_SIZE; y++)
    {
        for(x = 0; x < WORLD_SIZE; x++)
        {
            if(randf() > 0.8f)
            {
                tile_world[y][x] = rand() % 17;
            }
            else
            {
                tile_world[y][x] = rand() % 17;
            }
        }
    }
}

float location = 0;

int animation_running = 0;
int animation_frame = 2;
int direction = 1;

int airplane_pos_x = RASTER_WIDTH / 2;
int airplane_pos_y = RASTER_HEIGHT /2;

int airplane_speed = 120;

int hover_frames = 0;

typedef struct _bullet_t
{
    float x, y, dx, dy;
    int life;


} bullet_t;
#define MAX_BULLETS 250
bullet_t bullets[MAX_BULLETS];


void draw_bullets(rafgl_raster_t *raster)
{
    int i;
    bullet_t p;
    for(i = 0; i < MAX_BULLETS; i++)
    {
        p = bullets[i];
        if(p.life <= 0) continue;
        rafgl_raster_draw_line(raster, p.x - p.dx, p.y - p.dy, p.x, p.y, rafgl_RGB(255, 0, 0));
    }
}
void update_bullets(float delta_time)
{
    int i;
    for(i = 0; i < MAX_BULLETS; i++)
    {
        if(bullets[i].life <= 0) continue;

        bullets[i].life--;

        //Ukoliko budes hteo da ga siris tj SKUPLJAS
       // bullets[i].x += bullets[i].dx;
        //bullets[i].dx *= 0.5f;
        bullets[i].y += bullets[i].dy;
        bullets[i].dy *= 0.995f;
        bullets[i].dy -= 2;

    }
}


void render_tilemap(rafgl_raster_t *raster)
{
    int x, y;
    int x0 = camx / TILE_SIZE;
    int x1 = x0 + (raster_width / TILE_SIZE) + 1;
    int y0 = camy / TILE_SIZE;
    int y1 = y0 + (raster_height / TILE_SIZE) + 2;


    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(x1 < 0) x1 = 0;
    if(y1 < 0) y1 = 0;

    if(x0 >= WORLD_SIZE) x0 = WORLD_SIZE - 1;
    if(y0 >= WORLD_SIZE) y0 = WORLD_SIZE - 1;
    if(x1 >= WORLD_SIZE) x1 = WORLD_SIZE - 1;
    if(y1 >= WORLD_SIZE) y1 = WORLD_SIZE - 1;

    rafgl_raster_t *draw_tile;

    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
        {
            draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
            rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx, y * TILE_SIZE - camy - draw_tile->height + TILE_SIZE);
        }
    }




}


void main_state_init(GLFWwindow *window, void *args)
{



    rafgl_raster_init(&raster, raster_width, raster_height);

    int i;

    char tile_path[256];

    for(i = 0; i < NUMBER_OF_TILES; i++)
    {

        sprintf(tile_path, "res/NewTiles/%d.jpg", i);
        rafgl_raster_load_from_image(&tiles[i], tile_path);
    }


    init_tilemap();
    rafgl_texture_init(&texture);
}





void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    /* hendluj input */
//
  //  printf("%d %d ", airplane_pos_x, airplane_pos_y);

 animation_running = 1;
 if(game_data->keys_down[RAFGL_KEY_W] || game_data->keys_down[RAFGL_KEY_S] || game_data->keys_down[RAFGL_KEY_A] || game_data->keys_down[RAFGL_KEY_D] ){
    if(game_data->keys_down[RAFGL_KEY_W]){
            if(camy < -1520)
        airplane_pos_y = airplane_pos_y - airplane_speed * delta_time;
      printf("%d ",  airplane_pos_y);
    }
     if(game_data->keys_down[RAFGL_KEY_S])
    {
        //iz nekog razloga je islo duplo brze w u odnosu na s, kadaa pritisnemo w islo je po 2 piksela, ovde po 1
        //if camy <1520 znaci da ako dodje do donjeg dela rastera da ne pomera avion vise
        if(camy < 1520){
        airplane_pos_y = airplane_pos_y + airplane_speed * delta_time +1;
        }
            printf("%d ",  airplane_pos_y);

    }
     if(game_data->keys_down[RAFGL_KEY_A])
    {
         //  printf("%d ",  airplane_pos_x);

        airplane_pos_x = airplane_pos_x - airplane_speed * delta_time;
    }
     if(game_data->keys_down[RAFGL_KEY_D])
    {
            //printf("%d ",  airplane_pos_x);
        airplane_pos_x = airplane_pos_x + airplane_speed * delta_time + 1;

    }
 }
    else
    {
        animation_running = 0;
    }

    if(animation_running)
    {
        if(hover_frames == 0)
        {


            hover_frames =4;
        }
        else
        {

            hover_frames--;
            // samo pomeranje kamere
            if(game_data->keys_down[RAFGL_KEY_S]){
                    printf("%d ", camy );

                     camy +=10;
                   }else if(game_data->keys_down[RAFGL_KEY_W]){
                        printf("%d ", camy );
                     camy -=10;
                   }else if(game_data->keys_down[RAFGL_KEY_A]){
                     camx -=10;
                   }else if(game_data->keys_down[RAFGL_KEY_D]){
                     camx +=10;
                   }

            if(game_data->keys_down[RAFGL_KEY_S] || game_data->keys_down[RAFGL_KEY_W]){



            //Ukoliko idemo gore ili dole, da stavi na frejm 2, da ne ispadne da idemo ukrivo
                if(animation_frame >2 ){
                animation_frame--;
                }  if(animation_frame <2 ){
                animation_frame++;
                }

            //prolazimo kao kroz matricu,  ako idemo gore, pomeri se jedan red ka gore
             if(game_data->keys_down[RAFGL_KEY_W]&& hover_frames % 2 == 0 && direction >=1){
                direction--;

             }
             if(game_data->keys_down[RAFGL_KEY_S]&& hover_frames % 2 == 0 && direction <=2){
                direction++;
             }
        // ovaj else se odnosi na da li pritiskamo a ili d, u tom slucaju samo menajmo kolonu
        }else{
            animation_frame = animation_frame % 5;
              if(game_data->keys_down[RAFGL_KEY_A]&& hover_frames % 2 == 0 && animation_frame >= 1){
                animation_frame--;
             } if(game_data->keys_down[RAFGL_KEY_D]&& hover_frames % 2 == 0 &&  animation_frame <=3){
                animation_frame++;
             }

        }
        }

    }




//////
 int i, gen = 5, radius = 10;
 float angle, speed;
 if(game_data->keys_down[RAFGL_KEY_F])
    {


     //   printf("lol");
        for( i = 0; (i < MAX_BULLETS) && gen; i++)
        {
            if(bullets[i].life <= 0)
            {
                bullets[i].life = 15;
                bullets[i].x = airplane_pos_x + 98;
                bullets[i].y = airplane_pos_y + 52;

                angle = randf() * 4;
                speed = 5;
                bullets[i].dx = cosf(angle) * speed;
                bullets[i].dy = sinf(angle) * speed;
                gen--;

            }
        }

    }









/// ako oces da pomeras kameru misem
/*

    int x, y;

    float xn, yn;


    if(game_data->mouse_pos_x < raster_width / 10)
    {
        camx -=10;
    }

    if(game_data->mouse_pos_x > raster_width - raster_width / 10)
    {
        camx +=10;
    }

    if(game_data->mouse_pos_y < raster_height / 10)
    {
        camy -=10;
    }

    if(game_data->mouse_pos_y > raster_height - raster_height / 10)
    {
        camy +=10;
    }

    if(game_data->keys_pressed[RAFGL_KEY_O])
    {
        tile_world[selected_y][selected_x]++;
        tile_world[selected_y][selected_x] %= NUMBER_OF_TILES;
    }
*/

    render_tilemap(&raster);




 rafgl_raster_draw_spritesheet(&raster, &airplane, animation_frame, direction, airplane_pos_x, airplane_pos_y);
               draw_bullets(&raster);
               update_bullets(delta_time);

               //MENJANJE BOJA
               if(game_data->keys_down[RAFGL_KEY_K]){

rafgl_pixel_rgb_t sampled, resulting;
    for(int y = 0; y < raster_height; y++)
    {
        for(int x = 0; x < raster_width; x++)
        {
              sampled = pixel_at_m(raster, x, y);


            resulting.r = 255 - sampled.r/2;
            resulting.g = 255 - sampled.g /2;
            resulting.b = 255 - sampled.b;

            pixel_at_m(raster, x, y) = resulting;
            }
    }
               }

        rafgl_texture_load_from_raster(&texture, &raster);


}


void main_state_render(GLFWwindow *window, void *args)
{
    /* prikazi teksturu */
    rafgl_texture_show(&texture);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_texture_cleanup(&texture);

}
