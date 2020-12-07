#include "list.h"
#include "stdlib.h"
#include "stdio.h"
#include "stddef.h"

typedef struct component {
    int id;

    // This is used to link components together in their list
    struct list_head list_node;
} component_t;

typedef struct player {
    int x;
    int y;


    // This is used to link players together in the players list
    struct list_head list_node;

    // This is used so each player can have their own list of components
    struct list_head components;
} player_t;

// similar to std::vector<player_t> players in C++
// NOTE: That the list is not type-safe. e.g It does not accept
// only player_t structs
LIST_HEAD(players);

int main(void) {
    // Create the players
    int i;
    for (i = 0; i < 10; ++i) {
        // player_t* new_player = (player_t*)kmalloc(sizeof(player_t), GFP_KERNEL);
        player_t* new_player = (player_t*)malloc(sizeof(player_t));
        new_player->x = i;
        new_player->y = i;
        list_add_tail(&new_player->list_node, &players);
    }


    // Print the players
    {
        puts("===== Players before delete =====");
        struct  list_head *pos;
        int counter = 0;

        list_for_each(pos, &players) {
            // In here pos will be the list_head item in the corresponding player struct
            player_t* p = NULL;

            // Extract get a reference to the player_t for this iteration
            p = list_entry(pos, player_t, list_node);

            // Here we can check if (p == NULL) to make sure we got a real struct back

            printf("Player %d {%d, %d}\n", counter, p->x, p->y);
            counter++;
        }
    }

    // Delete specific player
    {
        struct list_head* pos = NULL;
        struct list_head* tmp;

        /*
           When modifying the structure of a list (i.e removing an item)
           use the _safe variant of the function. If you do not use the _safe
           versions you run the risk of losing part of your list.

           The _safe variants require an extra variable to temporary store
           some state for the list so you don't lose any of it.
        */
        list_for_each_safe(pos, tmp, &players) {
            player_t* p = list_entry(pos, player_t, list_node);

            if (p->x == 5 && p->y == 5) {
                /*
                   ALWAYS delete the item from the list first, then free it.
                   Not the other way around.
                */
                // pos has been modified here. If you need the original node, use tmp
                list_del(pos);
                // kfree(p)
                free(p);
            }
        }
    }


    {
        puts("===== Players after delete =====");
        struct list_head* pos = NULL;
        int counter = 0;
        // We can use the non safe version again since we do not plan on modifying the list structure
        list_for_each(pos, &players) {
            player_t* p = NULL;
            p = list_entry(pos, player_t, list_node);
            printf("Player %d {%d, %d}\n", counter, p->x, p->y);
            counter++;
        }
    }

    // Create "Component List"
    {
        struct  list_head *pos;
        list_for_each(pos, &players) {
            player_t* p = NULL;
            p = list_entry(pos, player_t, list_node);
            // Create a list at THIS player's (p) components variable
            INIT_LIST_HEAD(&p->components);
            int i;
            for (i = 0; i < 5; ++i) {
                component_t* comp = (component_t*)malloc(sizeof(component_t));
                comp->id = i;
                list_add_tail(&comp->list_node, &p->components);
            }
        }
    }

    puts("===== Players with components  =====");
    // Print players and components
    {
        struct  list_head *pos;
        int counter = 0;
        list_for_each(pos, &players) {
            player_t* p = NULL;
            p = list_entry(pos, player_t, list_node);
            printf("Player %d {%d, %d}\n", counter, p->x, p->y);
            component_t* the_component;
            /*
               A different way to iterate over the list. This gives us
               the item itself directly instead of having to call list_entry
               ourselves
            */
            list_for_each_entry(the_component, &p->components, list_node) {
                printf("\t Component id: %d\n", the_component->id);
            }
            counter++;
        }
    }

    {
        player_t* the_player;
        player_t* temp_player;
        list_for_each_entry_safe(the_player, temp_player, &players, list_node) {
            // Loop over the player's components
            component_t* the_component;
            component_t* temp_component;

            list_for_each_entry_safe(the_component, temp_component, &the_player->components, list_node) {
                // Remove the component from the list
                list_del(&the_component->list_node);
                // Free the memory
                free(the_component);
            }

            // Remove the player from the list
            list_del(&the_player->list_node);
            // Free the memory
            free(the_player);
        }
    }

    return 0;
}
