#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int id;
    char name[50];
    float price;
} MenuItem;

typedef struct
{
    int orderId;
    int itemId;
    char itemName[50];
    int quantity;
    float totalPrice;
} Order;

// Function Prototypes
void addMenuItem();
void displayMenu();
void searchMenuItem();
void editMenuItem();
void deleteMenuItem();
void placeOrder();
void displayOrders();

int main()
{
    int choice;

    do
    {
        printf("\n===== Restaurant Management System =====\n");
        printf("1. Add Menu Item\n");
        printf("2. Display All Menu Items\n");
        printf("3. Search Menu Item\n");
        printf("4. Edit Menu Item\n");
        printf("5. Delete Menu Item\n");
        printf("6. Place Order\n");
        printf("7. Display All Orders\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addMenuItem();
            break;
        case 2:
            displayMenu();
            break;
        case 3:
            searchMenuItem();
            break;
        case 4:
            editMenuItem();
            break;
        case 5:
            deleteMenuItem();
            break;
        case 6:
            placeOrder();
            break;
        case 7:
            displayOrders();
            break;
        case 8:
            printf("Exiting... Thank you!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);

    return 0;
}

// Function to add a menu item
void addMenuItem()
{
    FILE *file = fopen("menu.dat", "ab");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    MenuItem item;
    printf("Enter Item ID: ");
    scanf("%d", &item.id);
    printf("Enter Item Name: ");
    getchar(); // Clear newline character
    fgets(item.name, 50, stdin);
    item.name[strcspn(item.name, "\n")] = '\0'; // Remove newline
    printf("Enter Item Price: ");
    scanf("%f", &item.price);

    fwrite(&item, sizeof(MenuItem), 1, file);
    fclose(file);

    printf("Menu item added successfully!\n");
}

// Function to display all menu items
void displayMenu()
{
    FILE *file = fopen("menu.dat", "rb");
    if (file == NULL)
    {
        printf("No menu items available.\n");
        return;
    }

    MenuItem item;
    printf("\n----- Menu -----\n");
    while (fread(&item, sizeof(MenuItem), 1, file))
    {
        printf("ID: %d, Name: %s, Price: %.2f\n", item.id, item.name, item.price);
    }
    fclose(file);
}

// Function to search for a menu item
void searchMenuItem()
{
    FILE *file = fopen("menu.dat", "rb");
    if (file == NULL)
    {
        printf("No menu items available.\n");
        return;
    }

    int id;
    printf("Enter Item ID to search: ");
    scanf("%d", &id);

    MenuItem item;
    int found = 0;
    while (fread(&item, sizeof(MenuItem), 1, file))
    {
        if (item.id == id)
        {
            printf("ID: %d, Name: %s, Price: %.2f\n", item.id, item.name, item.price);
            found = 1;
            break;
        }
    }
    fclose(file);

    if (!found)
    {
        printf("Item not found.\n");
    }
}

// Function to edit a menu item
void editMenuItem()
{
    FILE *file = fopen("menu.dat", "rb+");
    if (file == NULL)
    {
        printf("No menu items available.\n");
        return;
    }

    int id;
    printf("Enter Item ID to edit: ");
    scanf("%d", &id);

    MenuItem item;
    int found = 0;
    while (fread(&item, sizeof(MenuItem), 1, file))
    {
        if (item.id == id)
        {
            printf("Enter New Name: ");
            getchar(); // Clear newline character
            fgets(item.name, 50, stdin);
            item.name[strcspn(item.name, "\n")] = '\0';
            printf("Enter New Price: ");
            scanf("%f", &item.price);

            fseek(file, -sizeof(MenuItem), SEEK_CUR); // Move pointer back
            fwrite(&item, sizeof(MenuItem), 1, file);
            found = 1;
            printf("Item updated successfully.\n");
            break;
        }
    }
    fclose(file);

    if (!found)
    {
        printf("Item not found.\n");
    }
}

// Function to delete a menu item
void deleteMenuItem()
{
    FILE *file = fopen("menu.dat", "rb");
    FILE *temp = fopen("temp.dat", "wb");
    if (file == NULL || temp == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    int id;
    printf("Enter Item ID to delete: ");
    scanf("%d", &id);

    MenuItem item;
    int found = 0;
    while (fread(&item, sizeof(MenuItem), 1, file))
    {
        if (item.id != id)
        {
            fwrite(&item, sizeof(MenuItem), 1, temp);
        }
        else
        {
            found = 1;
        }
    }
    fclose(file);
    fclose(temp);

    remove("menu.dat");
    rename("temp.dat", "menu.dat");

    if (found)
    {
        printf("Item deleted successfully.\n");
    }
    else
    {
        printf("Item not found.\n");
    }
}

// Function to place an order
void placeOrder()
{
    FILE *menuFile = fopen("menu.dat", "rb");
    FILE *orderFile = fopen("orders.dat", "ab");
    if (menuFile == NULL)
    {
        printf("No menu items available.\n");
        return;
    }
    if (orderFile == NULL)
    {
        printf("Error opening orders file.\n");
        fclose(menuFile);
        return;
    }

    int id, quantity;
    float total = 0;
    MenuItem item;
    Order order;
    static int orderCounter = 1; // Incremental order ID

    printf("Enter Item ID to order (0 to finish): ");
    while (scanf("%d", &id) && id != 0)
    {
        printf("Enter Quantity: ");
        scanf("%d", &quantity);

        rewind(menuFile); // Reset file pointer
        int found = 0;
        while (fread(&item, sizeof(MenuItem), 1, menuFile))
        {
            if (item.id == id)
            {
                total += item.price * quantity;
                printf("Added %d x %s to the bill.\n", quantity, item.name);

                // Save order to file
                order.orderId = orderCounter++;
                order.itemId = id;
                strcpy(order.itemName, item.name);
                order.quantity = quantity;
                order.totalPrice = item.price * quantity;
                fwrite(&order, sizeof(Order), 1, orderFile);

                found = 1;
                break;
            }
        }
        if (!found)
        {
            printf("Item not found.\n");
        }
        printf("Enter next Item ID (0 to finish): ");
    }
    fclose(menuFile);
    fclose(orderFile);

    printf("Total Bill: %.2f\n", total);
}

// Function to display all orders
void displayOrders()
{
    FILE *file = fopen("orders.dat", "rb");
    if (file == NULL)
    {
        printf("No orders available.\n");
        return;
    }

    Order order;
    printf("\n----- Orders -----\n");
    while (fread(&order, sizeof(Order), 1, file))
    {
        printf("Order ID: %d, Item ID: %d, Item Name: %s, Quantity: %d, Total Price: %.2f\n",
               order.orderId, order.itemId, order.itemName, order.quantity, order.totalPrice);
    }
    fclose(file);
}
