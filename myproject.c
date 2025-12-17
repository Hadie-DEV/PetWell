#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define MAX_HISTORY 500
#define MIN_ADD_QTY 5
#define MIN_PRICE 0.01
#define MAX_PRICE 10000
#define MIN_DISCOUNT 10
#define MAX_DISCOUNT 20
#define MIN_SELL_QTY 1
#define LOW_STOCK_THRESHOLD 5




/* ---------------- Data Structures ---------------- */
typedef struct {
    int id;
    char name[50];
    float price;
    float originalPrice;
    int quantity;
    float totalSales;
} Product;

Product products[MAX_PRODUCTS];
int productCount = 0;

char history[MAX_HISTORY][100];
int historyCount = 0;

/* ---------------- Helper Functions ---------------- */
Product* findProduct(int id) {
    for (int i = 0; i < productCount; i++)
        if (products[i].id == id)
            return &products[i];
    return NULL;
}

void logAction(const char* action) {
    if (historyCount < MAX_HISTORY)
        strcpy(history[historyCount++], action);
}

void saveProducts() {
    FILE* fp = fopen("products.dat", "wb");
    if (!fp) return;
    fwrite(products, sizeof(Product), productCount, fp);
    fclose(fp);
}

void loadProducts() {
    FILE* fp = fopen("products.dat", "rb");
    if (!fp) return;
    productCount = fread(products, sizeof(Product), MAX_PRODUCTS, fp);
    fclose(fp);
}

/* ---------------- Input Helpers ---------------- */
int getPositiveID(const char* prompt) {
    int id;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &id) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
        } else if (id <= 0) {
            printf("ID must be greater than 0.\n");
        } else {
            while (getchar() != '\n');
            return id;
        }
    }
}

int getQuantityToAdd() {
    int qty;
    while (1) {
        printf("Enter quantity to add (>%d): ", MIN_ADD_QTY);
        if (scanf("%d", &qty) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
        } else if (qty <= MIN_ADD_QTY) {
            printf("Quantity must be greater than %d.\n", MIN_ADD_QTY);
        } else {
            while (getchar() != '\n');
            return qty;
        }
    }
}

int getQuantityToSell(int stock) {
    int qty;
    while (1) {
        printf("Enter quantity to sell: ");
        if (scanf("%d", &qty) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
        } else if (qty < MIN_SELL_QTY) {
            printf("Quantity must be at least %d.\n", MIN_SELL_QTY);

        } else if (qty > stock) {
            printf("Not enough stock! Maximum available: %d\n", stock);
        } else {
            while (getchar() != '\n');
            return qty;
        }
    }
}

float getFloatInRange(const char* prompt, float min, float max) {
    float value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%f", &value) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n');
        } else if (value < min || value > max) {
            printf("Input must be between %.2f and %.2f.\n", min, max);
        } else {
            while (getchar() != '\n');
            return value;
        }
    }
}

void getValidMedicineName(char* name) {
    char temp[50];
    while (1) {
        printf("Enter Medicine Name: ");
        fgets(temp, 50, stdin);
        temp[strcspn(temp, "\n")] = 0;

        int hasLetter = 0;
        for (int i = 0; temp[i]; i++) {
            if ((temp[i] >= 'A' && temp[i] <= 'Z') || (temp[i] >= 'a' && temp[i] <= 'z')) {
                hasLetter = 1;
                break;
            }
        }

        if (!hasLetter) {
            printf("Medicine name must contain at least one letter.\n");
        } else {
            strcpy(name, temp);
            break;
        }
    }
}

/* ---------------- Core Functions ---------------- */
void addMedicine() {
    if (productCount >= MAX_PRODUCTS) {
        printf("Inventory full!\n\n");
        return;
    }

    Product p;
    char buffer[100];

    p.id = getPositiveID("Enter Medicine ID: ");
    getValidMedicineName(p.name);
    p.price = getFloatInRange("Enter Price per Unit: ", MIN_PRICE, MAX_PRICE);
    p.originalPrice = p.price;
    p.quantity = getQuantityToAdd();
    p.totalSales = 0;

    products[productCount++] = p;

    sprintf(buffer, "Added medicine: %s, ID:%d", p.name, p.id);
    logAction(buffer);

    printf("Medicine added successfully!\n\n");
}

void viewMedicines() {
    if (productCount == 0) {
        printf("No medicines available.\n\n");
        return;
    }

    printf("\n--- MEDICINE LIST ---\n");
    printf("%-5s %-20s %-10s %-10s\n", "ID", "Name", "Quantity", "Price");
    for (int i = 0; i < productCount; i++) {
        printf("%-5d %-20s %-10d $%-10.2f", products[i].id, products[i].name, products[i].quantity, products[i].price);
        #define LOW_STOCK_THRESHOLD 5
        if (products[i].quantity < LOW_STOCK_THRESHOLD)
        printf("  [LOW STOCK]");


        printf("\n");
    }
    printf("\n");
}

void updateMedicineStock() {
    char buffer[100];
    int id = getPositiveID("Enter Medicine ID to update: ");
    Product* p = findProduct(id);
    if (!p) { printf("ID not found!\n\n"); return; }

    int qty = getQuantityToAdd();
    p->quantity += qty;

    sprintf(buffer, "Added %d to %s (ID:%d)", qty, p->name, id);
    logAction(buffer);

    printf("Stock updated!\n\n");
}

void sellMedicine() {
    char buffer[100];
    int id = getPositiveID("Enter Medicine ID to sell: ");
    Product* p = findProduct(id);
    if (!p) { printf("ID not found!\n\n"); return; }

    int qty = getQuantityToSell(p->quantity);
    p->quantity -= qty;
    p->totalSales += qty * p->price;

    sprintf(buffer, "Sold %d of %s (ID:%d)", qty, p->name, id);
    logAction(buffer);

    printf("Medicine sold!\n\n");
}

void checkMedicineStock() {
    int id = getPositiveID("Enter Medicine ID to check stock: ");
    Product* p = findProduct(id);
    if (!p) { printf("ID not found!\n\n"); return; }

    printf("Stock for %s: %d\n\n", p->name, p->quantity);
}

void calculateStockValue() {
    float total = 0;
    for (int i = 0; i < productCount; i++)
        total += products[i].quantity * products[i].price;

    printf("Total stock value: $%.2f\n\n", total);
}

void applyDiscount() {
    char buffer[100];
    int id = getPositiveID("Enter Medicine ID for discount: ");
    Product* p = findProduct(id);
    if (!p) { printf("ID not found!\n\n"); return; }

    float disc = getFloatInRange("Enter discount (10-20%): ", MIN_DISCOUNT, MAX_DISCOUNT);

    p->price = p->originalPrice * (1 - disc / 100);

    sprintf(buffer, "Applied %.2f%% discount to %s (ID:%d)", disc, p->name, id);
    logAction(buffer);

    printf("Discount applied! New price for %s: $%.2f\n\n", p->name, p->price);
}

void removeMedicine() {
    char buffer[100];
    int id = getPositiveID("Enter Medicine ID to remove: ");
    int found = 0, i;
    for (i = 0; i < productCount; i++) {
        if (products[i].id == id) { found = 1; break; }
    }
    if (!found) { printf("ID not found!\n\n"); return; }

    for (; i < productCount - 1; i++) products[i] = products[i + 1];
    productCount--;

    sprintf(buffer, "Removed medicine ID:%d", id);
    logAction(buffer);

    printf("Medicine removed!\n\n");
}

void viewStockHistory() {
    if (historyCount == 0) { printf("No actions recorded.\n\n"); return; }
    printf("\n--- STOCK MOVEMENT HISTORY ---\n");
    for (int i = 0; i < historyCount; i++)
        printf("%d. %s\n", i + 1, history[i]);
    printf("\n");
}

void generateReport() {
    if (productCount == 0) {
        printf("No medicines available to generate report.\n\n");
        return;
    }

    float total_value = 0;

    // Print header
    printf("%-5s %-25s %-7s %-10s %-12s\n", "ID", "Medicine Name", "Qty", "Price($)", "Stock Value($)");
    printf("--------------------------------------------------------------------------\n");

    // Print each medicine row
    for (int i = 0; i < productCount; i++) {
        float stock_value = products[i].quantity * products[i].price;
        total_value += stock_value;
        printf("%-5d %-25s %-7d %-10.2f %-12.2f\n",
               products[i].id,
               products[i].name,
               products[i].quantity,
               products[i].price,
               stock_value);
    }

    printf("--------------------------------------------------------------------------\n");
    printf("%-39s %-10s %-12.2f\n", "", "Total:", total_value);
}

/* ---------------- Main Menu ---------------- */
int main() {
    int choice;

    loadProducts();

    while (1) {
        printf("\n--- PetWell: Pet Care Stock & Medicine Management System ---\n");
        printf("%2d. Add Medicine\n", 1);
        printf("%2d. View Medicines\n", 2);
        printf("%2d. Update Medicine Stock\n", 3);
        printf("%2d. Sell Medicine\n", 4);
        printf("%2d. Check Medicine Stock\n", 5);
        printf("%2d. Calculate Stock Value\n", 6);
        printf("%2d. Apply Discount\n", 7);
        printf("%2d. Remove Medicine\n", 8);
        printf("%2d. Stock Movement History\n", 9);
        printf("%2d. Generate Report\n", 10);
        printf("%2d. Exit\n\n", 11); // extra space after menu

        choice = getPositiveID("Enter choice: ");

        switch (choice) {
            case 1: addMedicine(); break;
            case 2: viewMedicines(); break;
            case 3: updateMedicineStock(); break;
            case 4: sellMedicine(); break;
            case 5: checkMedicineStock(); break;
            case 6: calculateStockValue(); break;
            case 7: applyDiscount(); break;
            case 8: removeMedicine(); break;
            case 9: viewStockHistory(); break;
            case 10: generateReport(); break;
            case 11: saveProducts(); exit(0);
            default: printf("Invalid choice!\n\n");
        }
    }

    return 0;
}
