#include <bn_core.h>
#include <bn_display.h>
#include <bn_log.h>
#include <bn_keypad.h>
#include <bn_random.h>
#include <bn_rect.h>
#include <bn_sprite_ptr.h>
#include <bn_sprite_text_generator.h>
#include <bn_size.h>
#include <bn_string.h>
#include <bn_backdrop.h>

#include "bn_sprite_items_dot.h"
#include "bn_sprite_items_square.h"
#include "common_fixed_8x16_font.h"

// Pixels / Frame player moves at
static constexpr bn::fixed SPEED = 2;

// Boost speed
static constexpr bn::fixed BOOSTED_SPEED = 5;

// Boost duration
static constexpr int BOOST_DURATION_FRAMES = 300;
static constexpr int MAX_BOOSTS = 3;

// Width and height of the the player and treasure bounding boxes
static constexpr bn::size PLAYER_SIZE = {8, 8};
static constexpr bn::size TREASURE_SIZE = {8, 8};

// Full bounds of the screen
static constexpr int MIN_Y = -bn::display::height() / 2;
static constexpr int MAX_Y = bn::display::height() / 2;
static constexpr int MIN_X = -bn::display::width() / 2;
static constexpr int MAX_X = bn::display::width() / 2;

// new starting location for treasure and player
static constexpr int PLAYER_START_X = -50;
static constexpr int PLAYER_START_Y = 50;
static constexpr int TREASURE_START_X = 0;
static constexpr int TREASURE_START_Y = 0;

// Number of characters required to show the longest numer possible in an int (-2147483647)
static constexpr int MAX_SCORE_CHARS = 11;

// Score location
static constexpr int SCORE_X = 70;
static constexpr int SCORE_Y = -70;

// Maximum number of Snake Segments including the head
static constexpr int MAX_SEGMENTS = 64;
// frames between each position so it looks smooth
static constexpr int POSITION_STEP_FRAMES = 2;

int main()
{
    bn::core::init();

    bn::random rng = bn::random();

    // Will hold the sprites for the score
    bn::vector<bn::sprite_ptr, MAX_SCORE_CHARS> score_sprites = {};
    bn::sprite_text_generator text_generator(common::fixed_8x16_sprite_font);

    int score = 0;

    int boost_remaining = MAX_BOOSTS;

    int boost_duration_counter = 0;

    bn::fixed current_speed = SPEED;

    bn::sprite_ptr player = bn::sprite_items::square.create_sprite(PLAYER_START_X, PLAYER_START_Y);
    bn::sprite_ptr treasure = bn::sprite_items::dot.create_sprite(TREASURE_START_X, TREASURE_START_Y);

    // snakes body not including the head
    bn::vector<bn::sprite_ptr, MAX_SEGMENTS> body_segments;
    // using vector to store the exisiting position of the head so the body can follow
    static constexpr int MAX_TRAIL_POINTS = MAX_SEGMENTS * 8;
    bn::vector<bn::fixed_point, MAX_TRAIL_POINTS> head_positions;
    // spacing the body segments along the tail
    int position_step_counter = 0;

    // Backdrop Color
    bn::backdrop::set_color(bn::color(30, 0, 30));

    while (true)
    {
        // Move player with d-pad
        if (bn::keypad::left_held())
        {
            player.set_x(player.x() - current_speed);
        }
        if (bn::keypad::right_held())
        {
            player.set_x(player.x() + current_speed);
        }
        if (bn::keypad::up_held())
        {
            player.set_y(player.y() - current_speed);
        }
        if (bn::keypad::down_held())
        {
            player.set_y(player.y() + current_speed);
        }

        // Speed Boost
        if (bn::keypad::a_pressed() && (boost_remaining > 0) && (boost_duration_counter == 0))
        {
            boost_remaining--;
            boost_duration_counter = BOOST_DURATION_FRAMES;
        }

        current_speed = SPEED;
        if (boost_duration_counter > 0)
        {
            current_speed = BOOSTED_SPEED;
            boost_duration_counter--;
        }

        if (bn::keypad::start_pressed())
        {
            player.set_x(PLAYER_START_X);
            player.set_y(PLAYER_START_Y);
            score = 0;
            treasure.set_position(TREASURE_START_X, TREASURE_START_Y);
            boost_remaining = MAX_BOOSTS;
        }

        // Wrap player around screen edges
        if (player.x() < MIN_X)
        {
            player.set_x(MAX_X);
        }
        else if (player.x() > MAX_X)
        {
            player.set_x(MIN_X);
        }

        if (player.y() < MIN_Y)
        {
            player.set_y(MAX_Y);
        }
        else if (player.y() > MAX_Y)
        {
            player.set_y(MIN_Y);
        }

        // https://gvaliente.github.io/butano/classbn_1_1fixed__point__t.html
        // Explained Butano docs found on github pages used for positioning
        // if statement to increase size
        position_step_counter++;
        if (position_step_counter >= POSITION_STEP_FRAMES)
        {
            position_step_counter = 0;
            if (head_positions.size() < MAX_TRAIL_POINTS)
            {
                // push the head vector down by one element
                head_positions.push_back(bn::fixed_point());
            }
            // push the rest of the body down by one index point
            for (int i = head_positions.size() - 1; i > 0; --i)
            {
                head_positions[i] = head_positions[i - 1];
            }
            // keep our head stored at index 0
            head_positions[0] = bn::fixed_point(player.x(), player.y());
        }

        // The bounding boxes of the player and treasure, snapped to integer pixels
        bn::rect player_rect = bn::rect(player.x().round_integer(),
                                        player.y().round_integer(),
                                        PLAYER_SIZE.width(),
                                        PLAYER_SIZE.height());
        bn::rect treasure_rect = bn::rect(treasure.x().round_integer(),
                                          treasure.y().round_integer(),
                                          TREASURE_SIZE.width(),
                                          TREASURE_SIZE.height());

        // If the bounding boxes overlap, set the treasure to a new location an increase score
        if (player_rect.intersects(treasure_rect))
        {
            // Jump to any random point in the screen
            int new_x = rng.get_int(MIN_X, MAX_X);
            int new_y = rng.get_int(MIN_Y, MAX_Y);
            treasure.set_position(new_x, new_y);

            score++;
        }

        // Update score display
        bn::string<MAX_SCORE_CHARS> score_string = bn::to_string<MAX_SCORE_CHARS>(score);
        score_sprites.clear();
        text_generator.generate(SCORE_X, SCORE_Y,
                                score_string,
                                score_sprites);

        // Update RNG seed every frame so we don't get the same sequence of positions every time
        rng.update();

        bn::core::update();
    }
}