/* src/portfolio.c
 * Person A (feature/core)
 * Simple Portfolio skeleton (safer version, preserves external symbols)
 *
 * Notes:
 * - Keeps the original global symbols portfolio and count non-static
 *   so other files that reference them won't break.
 * - Improves input safety (fgets + parsing), validation, and string handling.
 * - Does not change any public function names or signatures.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_STOCKS 100
#define SYMBOL_LEN 16
#define LINE_BUF 128

typedef struct {
    char symbol[SYMBOL_LEN];
    int qty;
    double buy_price;
    double cur_price;
} Stock;

/* Global portfolio array (simple for semester 1)
 * KEEP non-static so other translation units still link correctly.
 */
Stock portfolio[MAX_STOCKS];
int count = 0;

/* ---------- Internal helpers (file-local) ---------- */

static void strtoupper(char *s) {
    for (; *s; ++s) *s = (char) toupper((unsigned char)*s);
}

/* safe line input, returns 1 on success, 0 on EOF */
static int get_line(char *buf, size_t n) {
    if (fgets(buf, (int)n, stdin) == NULL) return 0;
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    return 1;
}

/* parse int safely; returns 1 on success */
static int parse_int(const char *s, int *out) {
    char *end;
    long v = strtol(s, &end, 10);
    if (end == s || *end != '\0') return 0;
    *out = (int)v;
    return 1;
}

/* parse double safely; returns 1 on success */
static int parse_double(const char *s, double *out) {
    char *end;
    double v = strtod(s, &end);
    if (end == s || *end != '\0') return 0;
    *out = v;
    return 1;
}

/* Find index by symbol (portfolio stores upper-case) */
static int find_index(const char *sym) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(portfolio[i].symbol, sym) == 0) return i;
    }
    return -1;
}

/* ---------- Person A: implemented functions (unchanged APIs) ---------- */

/* Print current holdings in a neat table */
void view() {
    if (count == 0) {
        printf("Portfolio is empty.\n");
        return;
    }
    printf("%-10s %-6s %-10s %-10s %-12s %-8s\n",
           "Symbol", "Qty", "Buy", "Cur", "Mkt Value", "P/L%");
    for (int i = 0; i < count; ++i) {
        double mv = portfolio[i].cur_price * portfolio[i].qty;
        double cost = portfolio[i].buy_price * portfolio[i].qty;
        double pl_pct = (cost == 0.0) ? 0.0 : ((mv - cost) / cost) * 100.0;
        printf("%-10s %-6d %-10.2f %-10.2f %-12.2f %-7.2f%%\n",
               portfolio[i].symbol,
               portfolio[i].qty,
               portfolio[i].buy_price,
               portfolio[i].cur_price,
               mv,
               pl_pct);
    }
}

/* Compute and print basic portfolio metrics */
void metrics() {
    double total_cost = 0.0;
    double market_value = 0.0;
    for (int i = 0; i < count; ++i) {
        total_cost += portfolio[i].buy_price * portfolio[i].qty;
        market_value += portfolio[i].cur_price * portfolio[i].qty;
    }
    double unrealized = market_value - total_cost;
    double pct = (total_cost == 0.0) ? 0.0 : (unrealized / total_cost) * 100.0;

    printf("Total cost basis : %.2f\n", total_cost);
    printf("Market value      : %.2f\n", market_value);
    printf("Unrealized P/L    : %.2f\n", unrealized);
    printf("Portfolio return  : %.2f%%\n", pct);
}

/* ---------- Placeholders for teammates to implement (do not modify) ---------- */

/* Person B will implement buying logic here */
/* --------- Person B: BUY FUNCTION (safer implementation) ----------- */
void buy() {
    char line[LINE_BUF];
    char sym[SYMBOL_LEN];
    int q;
    double p;

    printf("Enter stock symbol: ");
    if (!get_line(line, sizeof(line))) return;
    if (strlen(line) == 0) { printf("No symbol entered.\n"); return; }
    snprintf(sym, sizeof(sym), "%s", line);
    strtoupper(sym);

    printf("Enter quantity: ");
    if (!get_line(line, sizeof(line)) || !parse_int(line, &q)) {
        printf("Invalid quantity.\n"); return;
    }
    if (q <= 0) { printf("Quantity must be > 0.\n"); return; }

    printf("Enter buy price: ");
    if (!get_line(line, sizeof(line)) || !parse_double(line, &p)) {
        printf("Invalid price.\n"); return;
    }
    if (p <= 0.0) { printf("Price must be > 0.\n"); return; }

    int idx = find_index(sym);
    if (idx >= 0) {
        double old_cost = (double)portfolio[idx].qty * portfolio[idx].buy_price;
        double new_cost = (double)q * p;
        portfolio[idx].qty += q;
        portfolio[idx].buy_price = (old_cost + new_cost) / (double)portfolio[idx].qty;
        portfolio[idx].cur_price = p;
        printf("Updated %s: qty=%d avg_buy=%.2f cur_price=%.2f\n",
               sym, portfolio[idx].qty, portfolio[idx].buy_price, portfolio[idx].cur_price);
        return;
    }

    if (count < MAX_STOCKS) {
        /* safe copy */
        snprintf(portfolio[count].symbol, SYMBOL_LEN, "%s", sym);
        portfolio[count].qty = q;
        portfolio[count].buy_price = p;
        portfolio[count].cur_price = p;
        count++;
        printf("Added %s to portfolio (qty=%d @ %.2f)\n", sym, q, p);
    } else {
        printf("Portfolio full! Cannot buy.\n");
    }
}

/* --------- Person B: SELL FUNCTION (safer implementation) ----------- */
void sell() {
    char line[LINE_BUF];
    char sym[SYMBOL_LEN];
    int q;
    double p;

    printf("Enter stock symbol: ");
    if (!get_line(line, sizeof(line))) return;
    if (strlen(line) == 0) { printf("No symbol entered.\n"); return; }
    snprintf(sym, sizeof(sym), "%s", line);
    strtoupper(sym);

    int index = find_index(sym);
    if (index == -1) {
        printf("Stock not found!\n");
        return;
    }

    printf("Enter quantity to sell: ");
    if (!get_line(line, sizeof(line)) || !parse_int(line, &q)) {
        printf("Invalid quantity.\n"); return;
    }
    if (q <= 0) { printf("Quantity must be > 0.\n"); return; }

    printf("Enter sell price: ");
    if (!get_line(line, sizeof(line)) || !parse_double(line, &p)) {
        printf("Invalid price.\n"); return;
    }
    if (p < 0.0) { printf("Price must be >= 0.\n"); return; }

    if (q > portfolio[index].qty) {
        printf("You don't have enough shares!\n");
        return;
    }

    portfolio[index].qty -= q;
    portfolio[index].cur_price = p;

    if (portfolio[index].qty == 0) {
        /* remove stock from array */
        for (int j = index; j < count - 1; j++) {
            portfolio[j] = portfolio[j + 1];
        }
        count--;
        printf("All shares sold. Stock removed.\n");
    } else {
        printf("Sold %d shares of %s. Remaining qty=%d\n", q, sym, portfolio[index].qty);
    }
}

/* Person C will implement price updates here */
void update_prices() {
    printf("[Person C] update_prices() not implemented yet.\n");
}

/* Person C will implement save/load persistence here */
void save_file() {
    printf("[Person C] save_file() not implemented yet.\n");
}

void load_file() {
    printf("[Person C] load_file() not implemented yet.\n");
}

/* Person D will implement UI improvements and docs */
void ui_help() {
    printf("[Person D] ui_help() not implemented yet.\n");
}

/* Simple menu (minimal) */
int menu() {
    char line[LINE_BUF];
    printf("\n1) View  2) Buy  3) Sell  4) Update Prices\n");
    printf("5) Metrics  6) Save  7) Load  0) Exit\n");
    printf("Choice: ");
    if (!get_line(line, sizeof(line))) return -1;
    int c;
    if (!parse_int(line, &c)) return -1;
    return c;
}

/* main loop */
int main(void) {
    int choice;
    /* load_file();  -- Person C will implement persistence later */
    while ((choice = menu()) != 0) {
        switch (choice) {
            case 1: view(); break;
            case 2: buy(); break;
            case 3: sell(); break;
            case 4: update_prices(); break;
            case 5: metrics(); break;
            case 6: save_file(); break;
            case 7: load_file(); break;
            default: printf("Invalid choice.\n"); break;
        }
    }
    /* save_file(); -- Person C will enable this later */
    printf("Goodbye!\n");
    return 0;
}
