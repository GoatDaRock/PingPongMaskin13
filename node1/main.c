#include <avr/io.h>

#include "spi.h"
#include "oled.h"
#include "adc.h"
#include <stdbool.h>
#include "uart.h"
#include "sram.h"
#include "io.h"
#include "can.h"
#include "mcp2515.h"

#define JOYSTICK_CAN_ID 0x10
#define START_GAME_CAN_ID 0x20
#define GAME_STATE_CAN_ID 0x30

void send_joystick_position_over_can(void)
{
    io_joystick_t joystick = io_read_joystick();
    adc_data_t adc = adc_read_all();
    can_message_t joystick_message;
    joystick_message.id = JOYSTICK_CAN_ID;
    joystick_message.length = 3;
    joystick_message.data[0] = adc.joy_x;
    joystick_message.data[1] = adc.joy_y;
    joystick_message.data[2] = joystick.btn; // Optional: include joystick button state

    if (!can_send_message(&joystick_message))
    {
        printf("ERROR: CAN send failed for joystick message\r\n");
        return;
    }
    printf("Sent joystick position over CAN: X=%u, Y=%u, Button=%u\r\n", adc.joy_x, adc.joy_y, joystick.btn);
}

void send_start_game_signal_over_can(void)
{
    can_message_t start_game_message;
    start_game_message.id = START_GAME_CAN_ID;
    start_game_message.length = 1;
    start_game_message.data[0] = 1; // Arbitrary data to indicate "start game"

    if (!can_send_message(&start_game_message))
    {
        printf("ERROR: CAN send failed for start game message\r\n");
        return;
    }
    printf("Sent start game signal over CAN\r\n");
}

void print_game_state(uint8_t lives, uint8_t score)
{
    oled_clear();
    oled_goto_line(0);
    oled_goto_column(0);
    oled_printf("PONG GAME");

    oled_goto_line(3);
    oled_goto_column(0);
    oled_printf("SCORE: %u", score);

    oled_goto_line(4);
    oled_goto_column(0);
    oled_printf("Lives: %u", lives);
}

uint8_t run_game(void)
{
    uint8_t lives = 3;
    uint8_t score = 0;

    print_game_state(lives, score);

    send_start_game_signal_over_can();

    can_message_t game_state_message;

    while (1)
    {
        send_joystick_position_over_can();
        if (can_receive_message(&game_state_message))
        {
            if (game_state_message.id == GAME_STATE_CAN_ID && game_state_message.length == 2)
            {
                lives = game_state_message.data[0];
                score = game_state_message.data[1];

                print_game_state(lives, score);

                if (lives == 0)
                {
                    return score; // Game over
                }
            }
        }
    }
}

int main(void)
{
    /*
     * UART for terminal output
     */
    uart_init();
    fdevopen(uart_putchar, uart_getchar);

    xmem_init();
    adc_init();

    /*
     * Initialize SPI
     */
    spi_init();

    oled_init();

    /*
     * Initialize MCP2515 in loopback mode
     */
    if (!can_init())
    {
        printf("ERROR: CAN initialization failed\r\n");
        return 1;
    }

    printf("CAN initialized successfully\r\n");

    /*
     * Create CAN message
     */
    can_message_t message;

    message.id = 0x05;
    message.length = 3;

    message.data[0] = 0x11;
    message.data[1] = 0x22;
    message.data[2] = 0x33;

    /*
     * Send test CAN message
     */
    if (!can_send_message(&message))
    {
        printf("ERROR: initial CAN send failed\r\n");
        return 1;
    }

    io_joystick_t joystick;
    io_buttons_t buttons;

    char options[5][10] = {"START GAME", "TUNE MOTOR"};

    int selected = 0;
    int number_of_options = 2;
    uint8_t high_score = 0;
    bool menu_can_change = true;

write_display:
    /*
     * Clear old contents.
     */
    oled_clear();

    /*
     * LINE 0
     */
    oled_goto_line(0);
    oled_goto_column(0);

    oled_printf("USER INTERFACE: (%u)", selected + 1);
    for (int i = 0; i < number_of_options; i++)
    {
        oled_goto_line(i + 1);
        oled_goto_column(0);
        if (selected == i)
        {
            oled_printf("* ");
        }
        else
        {
            oled_printf(". ");
        }
        oled_printf(options[i]);
    }

    while (1)
    {

        adc_data_t adc = adc_read_all();
        uint8_t adc_y = adc.joy_y;

        joystick = io_read_joystick();

        buttons = io_read_buttons();

        oled_goto_line(6);
        oled_goto_column(0);
        oled_printf("HIGHSCORE: %3u  ", high_score);

        if (!menu_can_change)
        {
            if (adc_y > 120 && adc_y < 200)
            {
                menu_can_change = true;
            }
            continue;
        }
        if (adc_y > 200 && selected > 0)
        {
            selected--;
            menu_can_change = false;
            goto write_display;
        }
        if (adc_y < 120 && selected < number_of_options - 1)
        {
            selected++;
            menu_can_change = false;
            goto write_display;
        }

        if (joystick.btn == 1)
        {
            switch (selected)
            {
            case 0:
                uint8_t final_score = run_game();
                if (final_score > high_score)
                {
                    high_score = final_score;
                }
                break;
            case 1:
                printf("TUNE MOTOR selected\r\n");
                break;
            default:
                break;
            }
        }
    }

    return 0;
}
