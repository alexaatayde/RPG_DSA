#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#define DELAY_MS 50
#define DILAY_MS 2


// ANSI Color Codes
#define RESET       "\x1b[0m"
#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define BOLDWHITE   "\x1b[1;37m"
#define GRAY        "\x1b[90m" 
#define ORANGE  "\033[38;5;208m"  
#define BLACK   "\033[30m"
#define BLINK   "\033[5m"
#define DGRAY   "\033[38;5;237m" 

// Struct for HeroClass
typedef struct {
    char name[20];
    float hpMultiplier;
    float attackMultiplier;
    float defenseMultiplier;
} HeroClass;

// Define constant hero classes
const HeroClass alex = {"Aile Eiks", 0.8, 1, 0.6};
const HeroClass dropout = {"Drew Weigh", 1.0, 1, 0.6};
const HeroClass Jollylady = {"Hwei Grout", 1.2, 1.1, 0.7};  

// Inventory using linked list
typedef struct InventoryItem {
    char name[20];
    char description[255];
    struct InventoryItem* next;
} InventoryItem;

// Struct for Character
typedef struct {
    char name[30];
    int level;
    int hp;
    int attack;
    int defence;
    int exp;
    HeroClass classType;
    InventoryItem* inventory;
    
    int storyProgress;
    char saveFileName[500];
    int inventoryCount;  

    int skillActive;  
    float skillMultiplier;  
    int turnsRemaining;  
} Character;

// Struct for Enemy
typedef struct {
    char name[30];
    int hp;
    int attack;
    int defence;
    int itemDropChance;
    int triggeredLowHP; 
} Enemy;


// Function declarations

void combat(Character *player, int specificEnemyIndex);
void die(Character *player);
void levelup(Character *player);
void enemyInteractables(Character *player, Enemy* enemy);
void resumeStory(Character *player);
void story(Character *player , Enemy* enemy);
void items(Character *player);
void textDelay(const char *text, int delay);


void textDilay(const char *text, int dilay)
{
	int i;
	for(i = 0; text[i] != '\0'; i++)
	{
		putchar(text[i]);
		fflush(stdout);
		usleep(dilay * 90);
	}
}
// Function for text animation delay without borders
void textDelay(const char *text, int delay) {
	
    
    	int i;
    for (i = 0; text[i] != '\0'; i++) {
        putchar(text[i]);
        fflush(stdout);
        usleep(delay * 100);  
        
		}
}

// Function to add item to inventory
void addItem(Character* player, const char* itemName, const char* description) {
    InventoryItem* newItem = (InventoryItem*)malloc(sizeof(InventoryItem));
    if (!newItem) return;
    strcpy(newItem->name, itemName);
    strcpy(newItem->description, description); 
    newItem->next = player->inventory;
    player->inventory = newItem;
    textDelay("You found an item: ", DELAY_MS);
    textDelay(itemName, DELAY_MS);
    textDelay("!\n", DELAY_MS);
    textDelay("Description: ", DELAY_MS);
    textDelay(description, DELAY_MS);
    textDelay("\n", DELAY_MS);
}
//funtion to create item descriptions
InventoryItem* createItem(const char* name, const char* description) {
    InventoryItem* newItem = (InventoryItem*)malloc(sizeof(InventoryItem));
    strcpy(newItem->name, name);
    strcpy(newItem->description, description);
    newItem->next = NULL;
    return newItem;
}


// Function to simulate rolling dice
int roll_dice(int num, int sides) {
    int total = 0;
    int i;
    for (i = 0; i < num; i++) {
        total += (rand() % sides) + 1;
    }
    return total;
}

// Function to create character
Character createCharacter(const char *name, int level, HeroClass classType) {
    Character c;
    strcpy(c.name, name);
    c.level = level;
    c.hp = (20 + (level * 10)) * classType.hpMultiplier;
    c.attack = (roll_dice(4, 6) + 8) * classType.attackMultiplier;
    c.defence = (roll_dice(3, 8) + 2) * classType.defenseMultiplier;


    c.exp = 0;
    c.classType = classType;
    c.inventory = NULL;
    c.skillActive = 0;
    c.skillMultiplier = 0;
    c.turnsRemaining = 0;
    return c;
}

// Function to generate a random enemy
Enemy generateEnemy(int specificEnemyIndex) {
    const char *enemyNames[] = {"Mang puto",  "Librong James","dogdog", "Daruil the Real"};
    int index;
	if (specificEnemyIndex >= 0 && specificEnemyIndex <= 4)
	{
	 index = specificEnemyIndex;
	 } 
	 else
	 {
	 	index = rand() % 5;
	 }
    Enemy e;
    strcpy(e.name, enemyNames[index]);
    e.hp = roll_dice(3, 8) + (5 + (index * 4));
   	e.attack = roll_dice(1, 5) + 15 + (index);    
    e.defence = roll_dice(2, 3) + 2;
    e.triggeredLowHP = 0;
    e.itemDropChance = 30; 
    return e;
}

void items (Character* player)
{
	 InventoryItem* current = player->inventory;
	 if (current == NULL) {
        textDelay("No item found!\n", DELAY_MS);
        return;
    }
    
	  if (strcmp(current->name, "Puto") == 0) {
        player->hp += 50; 
        textDelay("You ate the Puto! Your HP is now ", DELAY_MS);
        printf("%d.\n", player->hp);
    }
   else if(strcmp(current->name,"Book Shard")==0)
    {
   	 player->skillActive = 99;            
        player->turnsRemaining = 3;         
        textDelay("You used the Book Shard!. you will take reduced damage for 3 turns\n", DELAY_MS);
    
	}
	else if(strcmp(current->name,"Datu Puti")==0)
	{
		textDelay("it is just an endgame relic, it does nothing .. unless ", DELAY_MS);
	}
}
// Function to use an item from inventory
void useItem(Character* player) {
    if (player->inventory == NULL) {
        textDelay("You have no items to use!\n", DELAY_MS);
        return;
    }

    textDelay("Choose an item to use:\n", DELAY_MS);
    InventoryItem* current = player->inventory;
    int itemIndex = 1;
    while (current != NULL) {
        printf("%d. %s\n", itemIndex, current->name);
        current = current->next;
        itemIndex++;
    }

    int choice;
    scanf("%d", &choice);


    current = player->inventory;
    int i;
    for ( i = 1; i < choice; i++) {
        current = current->next;
        if (current == NULL) return; 
    }

  	textDelay("You selected: ", DELAY_MS);
    textDelay(current->name, DELAY_MS);
    textDelay("\nDescription: ", DELAY_MS);
    textDelay(current->description, DELAY_MS);
    textDelay("\n", DELAY_MS);
	items(player);
	
    // Remove item logic 
    if (current == player->inventory) {
        player->inventory = current->next;
    } else {
        InventoryItem* prev = player->inventory;
        while (prev->next != current) {
            prev = prev->next;
        }
        prev->next = current->next;
    }
    free(current);
}

void levelup(Character* player)
{
	if (player->hp > 0) {
            textDelay("\nYou won the battle!\n", DELAY_MS);
            system("cls");  
            player->exp += 10;
            if (player->exp >= 20) {
                player->level++;
                player->exp = 0;
                textDelay("Level up!\n", DELAY_MS);
                 player->hp += 5 * player->classType.hpMultiplier;
            	player->attack += 2 * player->classType.attackMultiplier;
            	player->defence += 1 * player->classType.defenseMultiplier;
            	printf("Your stats improved! HP: %d, Attack: %d, Defense: %d\n", 
                   player->hp, player->attack, player->defence);
                Sleep(1000);
                system("cls");
            }
        }
}
void gameOver(Character *player)
{
	textDelay("thank you for playing", DELAY_MS);
	player->hp = 0;
}
void die(Character *player) {
    textDelay("\nYou succumb to your fate...\n", DELAY_MS);
    textDelay("The journey ends here.\n", DELAY_MS);
    player->hp = 0;
	
}
void enemyInteractables(Character *player, Enemy* enemy)
{
 if (strcmp(enemy->name, "Mang puto") == 0) {
 		Sleep(1000);
 		system("cls");
        textDelay(RED "\nputo in a mall\n" RESET, DELAY_MS);
        enemy->hp += 20;
        textDelay(RED "\nMang Puto's health boosted!\n" RESET, DELAY_MS);
    } 
    else if (strcmp(enemy->name, "Librong James") == 0) {
    	Sleep(1000);
 		system("cls");
        textDelay(RED "\n read my books m-...\n" RESET, DELAY_MS);
        enemy->defence += 20;
        textDelay(RED "\nLibrong jame's defense boosted'!\n" RESET, DELAY_MS);
        
    } 
    
}

void combat(Character *player, int specificEnemyIndex) {
    int round = 1; 
    int choice;
    	Enemy enemy = generateEnemy(specificEnemyIndex);
    	
		printf(ORANGE "Round %d\n" RESET, round++);

	

		printf(RED "Enemy: %s " RESET "| " YELLOW "HP: %d" RESET " | " RED "Attack: %d" RESET " | " BLUE "Defense: %d\n" RESET,
       	enemy.name, enemy.hp, enemy.attack, enemy.defence);

		printf(GREEN "%s Stats: " RESET "|" YELLOW " HP: %d " RESET "|" RED " Attack: %d" RESET "|" BLUE " Defense: %d\n"RESET, player->name, player->hp, player->attack, player->defence);

		while (player->hp > 0 && enemy.hp > 0) {
    		textDelay("\n" MAGENTA "Choose an action:\n" RESET
              "1. Attack\n"
              "2. Use Item\n"
              "3. Use Skill\n"
              "4. Run\n", DELAY_MS);

    		
    			int choice = getch() - '0';
				
    		int damage = 0;

    	if (choice == 1) {
        	damage = (roll_dice(2, 6) + 5) * player->classType.attackMultiplier;

        if (player->skillActive) {
            damage *= player->skillMultiplier;
            printf(GREEN "\nSkill active! Attack damage is increased!\n" RESET);
            player->skillActive = 0;
        }

         int finalDamage = damage - (enemy.defence * 0.5);
            if (finalDamage < 5) finalDamage = 5;
        enemy.hp -= finalDamage;
        
        // low hp enemy unique dialouges and powerups
        if (enemy.hp <= 15 && enemy.triggeredLowHP == 0) {
    			enemyInteractables(player, &enemy);  
  		 		 enemy.triggeredLowHP = 1; 
			}
			
			//exclusive bruno condition
       	
			if (strcmp(enemy.name, "dogdog") == 0 && enemy.hp > 0) {
			    InventoryItem* currentItem = player->inventory;
			    while (currentItem != NULL) {
			        if (strcmp(currentItem->name, "Datu Puti") == 0) {
			            textDelay(RED "\nYou poured Datu Puti on dogdog...\n", DELAY_MS);
			            textDelay(RED "dogdog retreats \n" RESET, DELAY_MS);
			            enemy.hp = 0;
			            finalDamage = 0; 
			            break;
			        }
			        currentItem = currentItem->next;
			    }
			}



        printf(YELLOW "\nYou attack dealing %d damage!\n" RESET, finalDamage);
    	} else if (choice == 2) {
        useItem(player);
    } else if (choice == 3) {
        if (strcmp(player->classType.name, "Hwei Grout") == 0) {
            printf(BLUE "\nChoose a skill\n" RESET);
            textDelay("\n1. Gaslight\n\n" YELLOW "(skill description: 40%% chance to stagger enemy attack while increasing damage next turn)\n" RESET,DELAY_MS);
        	
            choice = getch() - '0';
            if (choice == 1) {
                player->skillActive = 1;
                player->skillMultiplier = 1;
                system("cls");
                printf(GREEN "\nYgot's skill activated!.\n" RESET);
                textDelay(GREEN "\nAw ikaw gud mamsir :(\n" RESET,DELAY_MS);
            }
        } else if (strcmp(player->classType.name, "Aile Eiks") == 0) {
        	 printf(BLUE "Choose a skill\n" RESET);
            textDelay("\n1.Caffeine overload \n\n" YELLOW "(Reinforce yourself, reduce damage taken for 2 turns)\n" RESET,DELAY_MS);
            scanf("%d", &choice);
            if (choice == 1) {
            player->skillActive = 2;
            player->turnsRemaining = 2;
              system("cls");
            printf(GREEN "\nAlex's skill activated! \n" RESET);
            textDelay(GREEN "\nHumanon nako ni ugma\n" RESET,DELAY_MS);
        		}
        } else if (strcmp(player->classType.name, "Drew Weigh") == 0) {
        	 textDelay("\n1.Cutting \n\n" YELLOW "(Increased damage and reduced damage for 1 turn)\n" RESET,DELAY_MS);
            scanf("%d", &choice);
            if (choice == 1) {
            player->skillActive = 3;
            player->turnsRemaining = 1;
              system("cls");
            printf(GREEN "\nDewey's skill activated! \n" RESET);
            textDelay(GREEN "\nSIR! CR sako\n" RESET,DELAY_MS);
        	}
        }
    } else if (choice == 4) {
    	Sleep(1000);
    	system("cls");
        if (roll_dice(1, 20) >= 18) {
            textDelay(GREEN "You successfully escape!\n" RESET, DELAY_MS);
            return;
        } else {
            textDelay(RED "You fail to escape! The fight continues.\n" RESET, DELAY_MS);
            
        }
    }

    Sleep(1000);
    system("cls");

    printf(BOLDWHITE "\nUpdated Stats:\n" RESET);
  	printf(RED "Enemy: %s " RESET "| " YELLOW "HP: %d" RESET " | " RED "Attack: %d" RESET " | " BLUE "Defense: %d\n" RESET,
    enemy.name, enemy.hp, enemy.attack, enemy.defence);

	printf(GREEN "%s Stats: " RESET "|" YELLOW " HP: %d " RESET "|" RED " Attack: %d" RESET "|" BLUE " Defense: %d\n"RESET, 
	player->name, player->hp, player->attack, player->defence);
	
    if ((player->skillActive == 2 || player->skillActive == 99) && player->turnsRemaining > 0) {
        enemy.attack /= 2;
        player->turnsRemaining--;
        printf(BLUE "Alex's shield is active! Enemy damage is reduced.\n" RESET);
    }

    if (player->skillActive == 3 && player->turnsRemaining > 0) {
        enemy.attack /= 2;
        player->attack += 2;
        player->turnsRemaining--;
        printf(BLUE "Dewey's skill is active!.\n" RESET);
    }

    if (player->skillActive && rand() % 100 < 40) {
        textDelay(MAGENTA "\nThe enemy's turn is skipped!\n" RESET, DELAY_MS);
    } else {
        if (enemy.hp > 0) {
         
                int enemyDamage = (enemy.attack * 0.9) - (player->defence * 0.7);
                float randomFactor = (float)(80 + rand() % 41) / 100.0;
                enemyDamage = (int)(enemyDamage * randomFactor);
                
               
               


            player->hp -= enemyDamage;
            printf(RED "Enemy attacks! dealing %d damage\n" RESET, enemyDamage);
            if (player->hp <= 0) {
    				player->hp = 0;
    				break; 
    			}
    	}
	}
}
	if (player->hp > 0 && enemy.hp <= 0) {
        levelup(player);
    }

}


char saveGame(Character *player, char *slot) {
    FILE *file;
    char filename[500];
    
    // Create a save filename 
<<<<<<< HEAD
    snprintf(filename, sizeof(filename), "%s", slot);
=======
    snprintf(filename, sizeof(filename), "save_game_%s", slot);
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d

    // Open the file in write text mode
    file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error saving game.\n");
        return 0;
    }

<<<<<<< HEAD
    
    // Notepad compatibility(Windows)
    //If using Sublime Text, (not necessary)
    fprintf(file, "\xEF\xBB\xBF");  // UTF-8 BOM

    // Write player data to the file 
=======
    // Notepad compatibility
    fprintf(file, "\xEF\xBB\xBF");  // UTF-8 BOM

    // Write player data to the file in a readable format
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
    fprintf(file, "Name: %s\n", player->name);
    fprintf(file, "HP: %d\n", player->hp);
    fprintf(file, "Story Progress: %d\n", player->storyProgress);
    fprintf(file, "XP: %d\n", player->exp);
    fprintf(file, "Level: %d\n", player->level);
<<<<<<< HEAD
    fprintf(file, "Class: %s\n", player->classType.name); 
=======
    fprintf(file, "Class: %s\n", player->classType.name); // Saving class name
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
    fprintf(file, "Attack: %d\n", player->attack);
    fprintf(file, "Defence: %d\n", player->defence);

    
    int count = 0;
    InventoryItem* current = player->inventory;
    while (current != NULL) {
        fprintf(file, "Item: %s|%s\n", current->name, current->description);
        current = current->next;
        count++;
    }
<<<<<<< HEAD
    fprintf(file, "Inventory Count: %d\n", count);  
=======
    fprintf(file, "Inventory Count: %d\n", count);  // Save actual count at the end
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
    

    fclose(file);
    printf("Game saved successfully to %s!\n", filename);
    Sleep(5000);
    system("cls");
    
    //resumeStory(player);
    return 1;
}

char loadGame(Character *player, char *slot) {
    FILE *file;
    char filename[500];
    char line[100];

    // Create a load filename 
<<<<<<< HEAD
    snprintf(filename, sizeof(filename), "%s", slot);
=======
    snprintf(filename, sizeof(filename), "save_game_%s.txt", slot);
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d

    // Open the file in read text mode
    file = fopen(filename, "r");
    if (file == NULL) {
<<<<<<< HEAD
        printf("No save game found in %s\n", slot);
=======
        printf("No save file found in slot %s.\n", slot);
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
        Sleep(4000);
        system("cls");
        return 0;
    }


    
    player->inventory = NULL;
    player->inventoryCount = 0;

    // Read player data from the file
    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "Name: ", 6) == 0) {
            sscanf(line + 6, "%[^\n]", player->name);  
        } else if (strncmp(line, "HP: ", 4) == 0) {
            sscanf(line + 4, "%d", &player->hp);
        } else if (strncmp(line, "Story Progress: ", 16) == 0) {
            sscanf(line + 16, "%d", &player->storyProgress);
        } else if (strncmp(line, "XP: ", 4) == 0) {
            sscanf(line + 4, "%d", &player->exp);
        } else if (strncmp(line, "Level: ", 7) == 0) {
            sscanf(line + 7, "%d", &player->level);
        } else if (strncmp(line, "Class: ", 7) == 0) {
            sscanf(line + 7, "%[^\n]", player->classType.name);  
        } else if (strncmp(line, "Attack: ", 8) == 0) {
            sscanf(line + 8, "%d", &player->attack);
        } else if (strncmp(line, "Defence: ", 9) == 0) {
            sscanf(line + 9, "%d", &player->defence);
        }else if (strncmp(line, "Item: ", 6) == 0) {
            InventoryItem* newItem = (InventoryItem*)malloc(sizeof(InventoryItem));
            if (newItem) {
                sscanf(line + 6, "%[^|]|%[^\n]", newItem->name, newItem->description);
                newItem->next = player->inventory;
                player->inventory = newItem;
                player->inventoryCount++;
            }
    }   }  

    fclose(file);
<<<<<<< HEAD
    // Store the loaded slot name
    strcpy(player->saveFileName, slot); 
=======
    strcpy(player->saveFileName, slot); // Store the loaded slot name

>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d

    // Confirm loaded data
    printf("\nGame loaded from file %s!\n", slot);
    Sleep(4000);
    system("cls");

    printf("Game Loaded Successfully!\n");
    Sleep(4000);
    system("cls");

    printf("Character Name: %s\n", player->name);
    printf("HP: %d\n", player->hp);
    printf("Story Progress: %d\n", player->storyProgress);
    printf("XP: %d\n", player->exp);
    printf("Level: %d\n", player->level); 
<<<<<<< HEAD
    printf("Class: %s\n", player->classType.name);  
=======
    printf("Class: %s\n", player->classType.name);  // Display class name
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
    printf("Attack: %d\n",player->attack);
    printf("Defence: %d",player->defence);

    InventoryItem* temp = player->inventory;
    printf("\nInventory:\n");
    while (temp != NULL) {
        printf("- %s: %s\n", temp->name, temp->description);
        temp = temp->next;
    }

    Sleep(6000);
    system("cls");
    
    resumeStory(player);
    return 1;
}


void showMainMenu() 
{
    printf("\n=== MAIN MENU ===\n");
    printf("[1] New Game\n");
    printf("[2] Load Game\n");
    printf("[3] Quit\n");
}






void resumeStory(Character *player) {
    int choice;
    char slot[500];



    while (player->storyProgress < 3) {
        
        // Show story menu
      
        printf("\nWhat would you like to do next?\n");
        printf("[1] Continue\n");
        printf("[2] Save Game Progress\n");
        printf("[3] Main Menu\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        system("cls");
        	
        if (choice == 1) {
            
            system("cls");

            switch (player->storyProgress) {
                case 0:
                
  					
                        // Encounter 1
                    textDelay(YELLOW "+---------------------------------------------------+\n" RESET, DELAY_MS);
					textDelay(YELLOW "| You bid your family goodbye as they send you off. |\n" RESET, DELAY_MS);
					textDelay(YELLOW "| They watch you walk outside, coins clinking with  |\n" RESET, DELAY_MS);
					textDelay(YELLOW "| each step.                                        |\n" RESET, DELAY_MS);
					textDelay(YELLOW "|                                                   |\n" RESET, DELAY_MS);
					textDelay(YELLOW "|      This is your journey, and yours alone.       |\n" RESET, DELAY_MS);
					textDelay(YELLOW "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
					
					textDelay(CYAN "Press any key to continue..." RESET, DELAY_MS);
					getch();
					system("cls");
					
					textDelay(YELLOW "+---------------------------------------------------+\n" RESET, DELAY_MS);
					textDelay(YELLOW "| As you step out, you are greeted with the fresh   |\n" RESET, DELAY_MS);
					textDelay(YELLOW "| aroma emanating from someone. A guy dressed in    |\n" RESET, DELAY_MS);
					textDelay(YELLOW "| casual attire walks up with a basket full of      |\n"RESET, DELAY_MS);
					textDelay(YELLOW "| freshly baked puto.                               |\n"RESET, DELAY_MS);
					textDelay(YELLOW "| He offers you one...                              |\n" RESET, DELAY_MS);
					textDelay(YELLOW "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
                        
                        textDelay(GREEN "+-------------------------------+\n", DELAY_MS);
                        textDelay(GREEN "|        Do you accept?        |\n", DELAY_MS);
                        textDelay(GREEN "|         1. Yes               |\n", DELAY_MS);
                        textDelay(GREEN "|         2. No                |\n", DELAY_MS);
                        textDelay(GREEN "+-------------------------------+\n" RESET, DELAY_MS);

                        choice = getch() - '0';
                        system("cls");

                        if (choice == 1) {
                        textDelay(YELLOW "\n+--------------------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(YELLOW "| Manong Puto reaches for his icepick to harvest your organs   |\n" RESET, DELAY_MS);
                        textDelay(YELLOW "| as currency... He lunges.                                   |\n" RESET, DELAY_MS);
                        textDelay(YELLOW "+--------------------------------------------------------------+\n\n" RESET, DELAY_MS);
                            combat(player, 0);
                             if (player->hp == 0) {
                            return die(player);
                        }
                        addItem(player, "Puto", "Baked with love.");
                        textDelay(GREEN "\n+-------------------------+\n", DELAY_MS);
                        textDelay(GREEN "| You obtained:   Puto    |\n", DELAY_MS);
                        textDelay(GREEN "+-------------------------+\n" RESET, DELAY_MS);
                        printf("     .-'''''''-.     .-'''''''-.     .-'''''''-.\n");
						printf("   .'           '. .'           '. .'           '.\n");
						printf("  /   O       O   \\   O       O   \\   O       O   \\\n");
						printf(" :                :                :               :\n");
						printf(" |     PUTO       |   W/ CHEESE    |     PUTO      |\n");
						printf(" :                :                :               :\n");
						printf("  \\              / \\______________/ \\             /\n");
						printf("   '.__________.'   '.__________.'   '.__________.'\n");
                            Sleep(2000);
                            system("cls");
                        
                        } else {
                        textDelay(YELLOW "+--------------------------------------------------------------+\n", DELAY_MS);
                        textDelay(YELLOW "| You refused his offer. You noticed his expression            |\n", DELAY_MS);
                        textDelay(YELLOW "| dejected and hurt but he still thanked you for your time.    |\n", DELAY_MS);
                        textDelay(YELLOW "+--------------------------------------------------------------+\n" RESET, DELAY_MS);
                            printf("\n" CYAN "Press any key to continue..." RESET);
                            getch();
                            system("cls");
                        }

                        // Continue Journey
                        textDelay(GREEN "+----------------------------------------------+\n", DELAY_MS);
                        textDelay(GREEN "| You continue onwards, trekking toward your   |\n", DELAY_MS);
                        textDelay(GREEN "| task...                                      |\n", DELAY_MS);
                        textDelay(GREEN "+----------------------------------------------+\n" RESET, DELAY_MS);
                        
                        player->storyProgress = 1;
                        Sleep(1000);
                        system("cls");
                        
                    break;

                case 1:
                        // Encounter 2
                        textDelay(GREEN "+--------------------------------------------------------------+\n", DELAY_MS);
                        textDelay(GREEN "| You arrive at your barangay, greeted by the townsfolk.       |\n", DELAY_MS);
                        textDelay(GREEN "| But one particular individual catches your attention.        |\n" RESET, DELAY_MS);
                        textDelay(ORANGE    "| He looks at your uniform menacingly                          |\n", DELAY_MS);	
                        textDelay(ORANGE "+--------------------------------------------------------------+\n\n" RESET, DELAY_MS);

                        printf("\n" CYAN "Press any key to continue..." RESET);
                        getch();
                        system("cls");

                        textDelay(ORANGE "+-----------------------------------------+\n", DELAY_MS);
                        textDelay(ORANGE "| \"I hate bus-attired outfits...\" he said. |\n", DELAY_MS);
                        textDelay(ORANGE "+-----------------------------------------+\n\n" RESET, DELAY_MS);
                        combat(player, 1);
                        if (player->hp == 0) {
                            return die(player);
                        }
                        printf("  _____________\n");
						printf(" /             \\\n");
						printf("|   _________   |\n");
						printf("|  |         |  |\n");
						printf("|  |         |  |\n");
						printf("|  |         |  |\n");
						printf("|   ////////  __|\n");
						printf(" \\_____________/\n");

                        addItem(player, "Book Shard", "can be lethal if 1.2grams are consumed (0_0).");
                        textDelay(GREEN "\n+------------------------------+\n", DELAY_MS);
                        textDelay(GREEN "| You obtained:   Book Shard  |\n", DELAY_MS);
                        textDelay(GREEN "+------------------------------+\n" RESET, DELAY_MS);
                        Sleep(2000);
                        system("cls");
                        
                    //therealencounter
					textDelay(ORANGE "+---------------------------------------------------+\n" RESET, DELAY_MS);
					textDelay(ORANGE "| The decomposed body of Librong James attracted a  |\n" RESET, DELAY_MS);
					textDelay(RED "| hoarde of hound, among was a figure               |\n" RESET, DELAY_MS);
					textDelay(RED "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
					printf("\n" CYAN "Press any key to continue..." RESET);
					getch();
					system("cls");
					
					textDelay(RED    "+---------------------------------------------------+\n" RESET, DELAY_MS);
					textDelay(RED    "|                     helo                           |\n" RESET, DELAY_MS);
					textDelay(RED    "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
					
					textDelay(GREEN "+-------------------------------+\n", DELAY_MS);
					textDelay(GREEN "|        Response?             |\n", DELAY_MS);
					textDelay(GREEN "|         1. hello             |\n", DELAY_MS);
					textDelay(GREEN "|         2. ok                |\n", DELAY_MS);
					textDelay(GREEN "+-------------------------------+\n" RESET, DELAY_MS);
					choice = getch() - '0';
					
					if (choice == 1) {
					textDelay(RED "\n+------------------------------------------------------------+\n" RESET, DELAY_MS);
					textDelay(RED "| *leaves                                                      |\n" RESET, DELAY_MS);
					textDelay(RED "+--------------------------------------------------------------+\n\n" RESET, DELAY_MS);
					printf("\n" CYAN "Press any key to continue..." RESET);
                            getch();
                            system("cls");
					  
					} else {
					textDelay(RED "+--------------------------------------------------------------+\n", DELAY_MS);
					textDelay(RED "| ta spar                                                     |\n", DELAY_MS);
					textDelay(RED "+--------------------------------------------------------------+\n" RESET, DELAY_MS);
					  printf("\n" CYAN "Press any key to continue..." RESET);
                        getch();
                        system("cls");
					combat(player, 3);
					 if (player->hp == 0) {
                            return die(player);
                        }
					    printf("\n" CYAN "Press any key to continue..." RESET);
					    getch();
					    system("cls");
					}
                        
                        player->storyProgress = 2;
                      

                    break;
				
                case 2:
                        // Ending
                       textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(GREEN "| the journey nears its end...                       |\n" RESET, DELAY_MS);
                        textDelay(GREEN "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
                        
                        
                            textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                            textDelay(GREEN "| The only obstacle between you and your goal is a  |\n" RESET, DELAY_MS);
                            textDelay(GREEN "| wooden door that is very knockable                |\n" RESET, DELAY_MS);
                            textDelay(GREEN "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
                       
                        
                        textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(GREEN "| The door opens. An elderly woman steps out.       |\n" RESET, DELAY_MS);
                        textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        
                        printf(CYAN "\nPress any key to continue..." RESET);
                        getch();
                        system("cls");
                        
                        textDelay(MAGENTA "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(MAGENTA "| \"Unsay ato|\" she asked.                           |\n" RESET, DELAY_MS);
                        textDelay(MAGENTA "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        
                        textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(GREEN "| You reach for the gold coins your mother gave     |\n" RESET, DELAY_MS);
                        textDelay(GREEN "| you for this journey. You point at an item...     |\n" RESET, DELAY_MS);
                        textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        
                        printf(CYAN "\nPress any key to continue..." RESET);
                        getch();
                        system("cls");
                        
                        printf("        ||\n");
						printf("       |==|\n");
						printf("       |==|\n");
						printf("       |==|\n");
						printf("      /====\\\n");
						printf("     |      |\n");
						printf("     | DATU |\n");
						printf("     | PUTI |\n");
						printf("     |______|\n");
						printf("     |      |\n");
						printf("     |      |\n");
						printf("     |______|\n");

                        
                        addItem(player, "Datu Puti", "Your endgoal item to give to your \nawaiting mother so that she can make a mean ah adobo.");
                        textDelay(GREEN "+---------------------------------------------------+\n" RESET, DELAY_MS);
                        textDelay(GREEN "| Datu Puti is added to your inventory!             |\n" RESET, DELAY_MS);
                        textDelay(GREEN "+---------------------------------------------------+\n\n" RESET, DELAY_MS);
                        Sleep(2000);
                        system("cls");
                        if (player->hp >= 10)
                        {
                            printf("wait what whats happening *naging pusa* \n");
                            combat(player, 2);
                             if (player->hp == 0) {
                            return die(player);
                        }
                            return;
                            
                        }
                        else {
                            printf("GAME OVER!");
                            Sleep(2000);
                            

                        }
                        system("cls");
                        player->storyProgress = 3;

                    break;

                default:
                    printf("You have finished the story or progress is unknown.\n");
                return;
            }
                
        }else if (choice == 2) {
            
            
            // Save Game
            if (strlen(player->saveFileName) == 0) {
                printf("Enter file name to save the game: ");
                scanf(" %[^\n]", slot);
<<<<<<< HEAD
                // Save the file name 
                strcpy(player->saveFileName, slot);  
            } else {
                // Reuse previously saved file name
                strcpy(slot, player->saveFileName);  
=======
                strcpy(player->saveFileName, slot);  // Save the file name for next time
            } else {
                strcpy(slot, player->saveFileName);  // Reuse previously saved file name
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
                printf("Saving game to %s...\n", slot);
            }
        
            system("cls");
        
            if (saveGame(player, slot)) {
                printf("Proceeding to the game.......\n");
            } else {
                printf("Failed to save game.\n");
            }
        
            Sleep(4000);
            system("cls");

        } else if (choice == 3) {
            // Back to Main Menu
            printf("Going back to main menu...\n");
            Sleep(4000);
            system("cls");
            showMainMenu();
            system("cls");
            return;  
        } else {
            printf("Invalid choice!\n");
            Sleep(3000);
            system("cls");
            printf("Returning.......");
            Sleep(4000);
            system("cls");
            continue;
        } 
    }

    
     
}


void story(Character *player, Enemy* enemy){
    player->storyProgress = 0;
    resumeStory(player);
}





int main() 
{
    srand(time(NULL));
<<<<<<< HEAD
=======
    //int menuChoice;
>>>>>>> fd51da94b28e904d1d5429284c907fa4216c752d
    int classChoice;
    Character player;
	Enemy enemy;
    char saveSlot[500];
    int running = 1;


    printf("\n");
    printf(ORANGE BLINK"        *****        \n"RESET);
    printf(ORANGE"     **"DGRAY"       "ORANGE"**     \n"RESET);
    printf(RED BLINK"   **"DGRAY"           "RED"**   \n"RESET);
    printf(RED"  *"DGRAY"               "RED"*  \n"RESET);
    printf(RED BLINK" *"BOLDWHITE"     DARKTHRU    "RED"*  \n"RESET);
    printf(YELLOW BLINK" *"BOLDWHITE"      PONTI!     "YELLOW"*  \n"RESET);
    printf(YELLOW"  *"DGRAY"               "YELLOW"*  \n"RESET);
    printf(ORANGE BLINK"   **"DGRAY"           "ORANGE"**   \n"RESET);
    printf(ORANGE"     **"DGRAY"       "ORANGE"**     \n"RESET);
    printf(ORANGE BLINK"        *****        \n"RESET);
    printf("\n");
	
	Sleep(3000);
	system("cls");
   

    while (running)
    {
        int menuChoice;
        // Show main menu
        showMainMenu();
        printf("Enter your choice: ");
        scanf("%d", &menuChoice);
        system("cls");


        switch(menuChoice)
        {

            case 1:
            {
                int classChoice;

                do
                {
                        
		           textDelay("+---------------------------------------+\n" BOLDWHITE,DILAY_MS);
					textDelay("|         SELECT YOUR CHARACTER         |\n" BOLDWHITE,DILAY_MS);
					textDelay("+---------------------------------------+\n" ORANGE,DILAY_MS);
					    
					textDelay("+---------------------------------------+\n" ORANGE,DILAY_MS);
					textDelay("| 1. Ygot - The Jollyworker             |\n" ORANGE,DILAY_MS);
					textDelay("|    HP:  36   ATK: 19  DEF: 14         |\n" ORANGE,DILAY_MS);
					textDelay("|    Special: Gaslight - Chance to skip |\n" ORANGE,DILAY_MS);
					textDelay("|    enemy turn and boost next attack   |\n" ORANGE,DILAY_MS);
					textDelay("+---------------------------------------+\n" BLUE,DILAY_MS);
					printf("\n");
					
					textDilay("----------------------------------------+\n" BLUE,DILAY_MS);
					textDilay("| 2. Alex - The Crammer                 |\n" BLUE,DILAY_MS);
					textDilay("|    HP: 24   ATK: 27  DEF: 15          |\n" BLUE,DILAY_MS);
					textDilay("|    Special: Caffeine Overload -       |\n" BLUE,DILAY_MS);
					textDilay("|    Reduces damage taken for 2 turns   |\n" BLUE,DILAY_MS);
					textDilay("+---------------------------------------+\n" GREEN,DILAY_MS);
					printf("\n");
					
					textDilay("+---------------------------------------+\n" GREEN,DILAY_MS);
					textDilay("| 3. Dewey - The Dropout                |\n" GREEN,DILAY_MS);
					textDilay("|    HP: 30    ATK: 26   DEF: 15        |\n" GREEN,DILAY_MS);
					textDilay("|    Special: Cutting - Increased crit  |\n" GREEN,DILAY_MS);
					textDilay("|    chance and reduced damage for 1    |\n" GREEN,DILAY_MS);
					textDilay("+---------------------------------------+\n" RED,DILAY_MS);
					printf("\n");
					
					textDilay("+---------------------------------------+\n" RED,DILAY_MS);
					textDilay("| 4. Exit Game                          |\n" RED,DILAY_MS);
					textDilay("+---------------------------------------+" YELLOW,DILAY_MS);
			                        
                    printf("\n\n");
                        
                    printf("\nEnter your choice: ");
                    scanf("%d", &classChoice);                        
                    system("cls");
                        
                    if (classChoice == 1) 
                        player = createCharacter("Hwei Grout", 1, Jollylady);
                    else if (classChoice == 2) 
                        player = createCharacter("Aile Eiks", 1, alex);
                    else if (classChoice == 3) 
                        player = createCharacter("Drew Weigh", 1, dropout);
                    else{

                        textDelay("Exiting Game......\n", DELAY_MS);
                        Sleep(4000);
                        system("cls");
                        textDelay("Thanks for playing!\n", DELAY_MS);
                        running = 0;
                        Sleep(4000);
                        system("cls");
                        return 0;
                    } 
                            
                        
                    
                    system("cls");
                    story(&player,&enemy);
                    break;

                } while (player.hp > 0);
                
                textDelay("Game Over!\n", DELAY_MS);
                Sleep(4000);
                system("cls");
                break;
            }

            case 2:
            {
                printf("Enter file name to load game: ");
                scanf("%s", saveSlot);
                system("cls");

                if (loadGame(&player, saveSlot)) 
                {
                    story(&player, &enemy);
                } else{
                    textDelay("Failed to load game.\n", DELAY_MS);
                    
                }
                Sleep(4000);
                system("cls");
                break;
            }
        
        
            case 3:
            {
                textDelay("Thanks for playing!\n", DELAY_MS);
                running = 0;
                Sleep(4000);
                system("cls");
                break;

                default:
                printf("Invalid choice!\nTry again.......\n");
                Sleep(4000);
                system("cls");
                break;
            }
        }
    }
    return 0;
}
    
    
