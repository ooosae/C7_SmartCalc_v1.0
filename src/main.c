#include <cairo.h>
#include <gtk/gtk.h>

#include "smart_calc.h"

typedef struct {
  GtkWidget *entry;
  GtkWidget *output;
  GtkWidget *x_field;
  GtkWidget *l_edge;
  GtkWidget *r_edge;
  int flag;
} UserData;

typedef struct {
  GtkWidget *area;
  GtkWidget *entry;
  cairo_t *cr;
} PlotData;

typedef struct {
  GtkWidget *overpayment_field;
  GtkWidget *total_payment_field;
  GtkWidget *tree_view;
  GtkWidget *total_loan_amount;
  GtkWidget *month_term;
  GtkWidget *interest_rate;
  GtkWidget *type;
} CreditData;

typedef struct {
  GtkWidget *accrued_interest;
  GtkWidget *tax_amount;
  GtkWidget *amount_on_deposit;
  GtkWidget *deposit_amount;
  GtkWidget *month_term;
  GtkWidget *interest_rate;
  GtkWidget *tax_rate;
  GtkWidget *repayment_periodicity;
  GtkWidget *interest_capitalization;
  GtkWidget *tree_view_1;
  GtkWidget *tree_view_2;
  GtkTreeStore *tree_store_1;
  GtkTreeStore *tree_store_2;
} DepositData;

void button_calc_expr(GtkWidget *button, UserData *userData, gpointer data) {
  const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(userData->entry));

  double answer;
  char expression[strlen(entry_text) + 1];
  double x_value =
      (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(userData->x_field));
  strcpy(expression, entry_text);
  int rc = solve_expression(expression, x_value, &answer);

  gchar result[100];

  switch (rc) {
    case 0: {
      g_strlcpy(result, g_strdup_printf("%.6lf", answer), sizeof(result));
      break;
    }
    case 10: {
      g_strlcpy(result,
                "The power of zero is undefined for a negative exponent",
                sizeof(result));
      break;
    }
    case 11: {
      g_strlcpy(result, "Division by zero is undefined", sizeof(result));
      break;
    }
    case 12: {
      g_strlcpy(result, "Taking the root of a negative number is not allowed",
                sizeof(result));
      break;
    }
    case 13: {
      g_strlcpy(result, "At this point the tangent is undefined",
                sizeof(result));
      break;
    }
    case 14: {
      g_strlcpy(result, "The logarithm is undefined for non-positive numbers",
                sizeof(result));
      break;
    }
    case 15: {
      g_strlcpy(result,
                "Inverse cosine is undefined for values outside [-1, 1]",
                sizeof(result));
      break;
    }
    case 16: {
      g_strlcpy(result, "Inverse sine is undefined for values outside [-1, 1]",
                sizeof(result));
      break;
    }
    default: {
      g_strlcpy(result, "Input or another error", sizeof(result));
      break;
    }
  }
  gtk_entry_set_text(GTK_ENTRY(userData->output), result);
}

void draw_axes_and_grid(cairo_t *cr, double width, double height, double min_x,
                        double max_x, double min_y, double max_y, int count) {
  cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
  cairo_set_line_width(cr, 1);

  if (max_y - min_y < height && max_x - min_x < width) {
    for (double y = min_y; y <= max_y; y += (max_y - min_y) / count) {
      double y_normalized = height - (y - min_y) / (max_y - min_y) * height;
      cairo_move_to(cr, 0, y_normalized);
      cairo_line_to(cr, width, y_normalized);
      cairo_stroke(cr);
    }

    for (double x = min_x; x <= max_x; x += (max_x - min_x) / count) {
      double x_normalized = (x - min_x) / (max_x - min_x) * width;
      cairo_move_to(cr, x_normalized, 0);
      cairo_line_to(cr, x_normalized, height);
      cairo_stroke(cr);
    }
  }

  cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
  cairo_set_line_width(cr, 2);

  double x_axis = -min_y / (max_y - min_y) * height;
  cairo_move_to(cr, 0, height - x_axis);
  cairo_line_to(cr, width, height - x_axis);

  double y_axis = (0 - min_x) / (max_x - min_x) * width;
  cairo_move_to(cr, y_axis, 0);
  cairo_line_to(cr, y_axis, height);

  cairo_stroke(cr);
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  UserData *userData = (UserData *)data;
  if (userData->flag) {
    return FALSE;
  }

  const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(userData->entry));

  int width, height;
  gtk_widget_get_size_request(widget, &width, &height);

  double answer;
  char expression[strlen(entry_text) + 1];
  double x_value =
      (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(userData->x_field));
  strcpy(expression, entry_text);

  double coordinates[1000][2];
  double l =
      (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(userData->l_edge));
  double r =
      (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(userData->r_edge));

  double step = (r - l) / width;  // Шаг
  int rc = get_graph(expression, l, r, step, coordinates);

  gchar result[100];
  userData->flag = 1;
  switch (rc) {
    case 0: {
      g_strlcpy(result, "Graph", sizeof(result));
      userData->flag = 0;

      int width, height;
      gtk_widget_get_size_request(widget, &width, &height);

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_paint(cr);

      double min_x = coordinates[0][0];
      double max_x = coordinates[width - 1][0];
      double min_y = min_x;
      double max_y = max_x;

      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_paint(cr);

      draw_axes_and_grid(cr, width, height, min_x, max_x, min_y, max_y,
                         (int)(r - l));

      for (int i = 0; i < width; i++) {
        coordinates[i][0] =
            (coordinates[i][0] - min_x) / (max_x - min_x) * width;
        coordinates[i][1] =
            height - (coordinates[i][1] - min_y) / (max_y - min_y) * height;
      }

      cairo_set_source_rgb(cr, 1, 1, 1);
      cairo_set_line_width(cr, 2);
      cairo_stroke(cr);
      cairo_move_to(cr, coordinates[0][0], coordinates[0][1]);
      for (int i = 1; i < width; i++) {
        if (isnan(coordinates[i][1]) || coordinates[i][1] >= height ||
            coordinates[i][1] <= 0) {
          cairo_stroke(cr);
        } else {
          cairo_line_to(cr, coordinates[i][0], coordinates[i][1]);
        }
      }

      cairo_stroke(cr);
      break;
    }
    case 40: {
      g_strlcpy(result, "The right edge must exceed the left edge",
                sizeof(result));
      break;
    }
    case 10: {
      g_strlcpy(result,
                "The power of zero is undefined for a negative exponent",
                sizeof(result));
      break;
    }
    case 11: {
      g_strlcpy(result, "Division by zero is undefined", sizeof(result));
      break;
    }
    case 12: {
      g_strlcpy(result, "Taking the root of a negative number is not allowed",
                sizeof(result));
      break;
    }
    case 13: {
      g_strlcpy(result, "At this point the tangent is undefined",
                sizeof(result));
      break;
    }
    case 14: {
      g_strlcpy(result, "The logarithm is undefined for non-positive numbers",
                sizeof(result));
      break;
    }
    case 15: {
      g_strlcpy(result,
                "Inverse cosine is undefined for values outside [-1, 1]",
                sizeof(result));
      break;
    }
    case 16: {
      g_strlcpy(result, "Inverse sine is undefined for values outside [-1, 1]",
                sizeof(result));
      break;
    }
    default: {
      g_strlcpy(result, "Input or another error", sizeof(result));
      break;
    }
  }
  gtk_entry_set_text(GTK_ENTRY(userData->output), result);

  return FALSE;
}

void go_flag(GtkWidget *button, UserData *userData) { userData->flag = 0; }

void button_draw_graph(GtkWidget *button, PlotData *plotData) {
  gtk_widget_queue_draw(plotData->area);
}

void button_print(GtkWidget *button, GtkWidget *entry, gpointer data) {
  const gchar *text = gtk_button_get_label(GTK_BUTTON(button));
  const gchar *entry_text = gtk_entry_get_text(GTK_ENTRY(entry));

  gchar *new_text = g_strdup_printf("%s%s", entry_text, text);
  gtk_entry_set_text(GTK_ENTRY(entry), new_text);
  g_free(new_text);
}

void button_clear(GtkWidget *button, GtkWidget *entry, gpointer data) {
  gtk_entry_set_text(GTK_ENTRY(entry), "");
}

void button_calc_credit(GtkWidget *button, CreditData *creditData,
                        gpointer data) {
  double total_loan_amount = (double)gtk_spin_button_get_value(
      GTK_SPIN_BUTTON(creditData->total_loan_amount));
  int month_term =
      (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(creditData->month_term));
  double interest_rate = (double)gtk_spin_button_get_value(
      GTK_SPIN_BUTTON(creditData->interest_rate));
  gboolean is_a =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(creditData->type));
  char type = is_a ? 'a' : 'd';
  double payment_month[201];
  double overpayment;
  double total_payment;

  int rc = calc_credit(total_loan_amount, month_term, interest_rate, type,
                       payment_month, &overpayment, &total_payment);

  if (!rc) {
    GtkListStore *list_store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_DOUBLE);
    gtk_tree_view_set_model(GTK_TREE_VIEW(creditData->tree_view),
                            GTK_TREE_MODEL(list_store));

    GtkTreeIter iter;
    for (int i = 0; i < month_term; i++) {
      gtk_list_store_append(list_store, &iter);
      gtk_list_store_set(list_store, &iter, 0, i + 1, 1, payment_month[i], -1);
    }

    gchar *overpayment_str =
        g_strdup_printf("Overpayment:\n%.2lf", overpayment);
    gchar *total_payment_str =
        g_strdup_printf("Total payment:\n%.2lf", total_payment);
    gtk_label_set_text(GTK_LABEL(creditData->overpayment_field),
                       overpayment_str);
    gtk_label_set_text(GTK_LABEL(creditData->total_payment_field),
                       total_payment_str);

    g_free(overpayment_str);
    g_free(total_payment_str);
  }
}

void button_calc_deposit(GtkWidget *button, DepositData *depositData,
                         gpointer data) {
  double deposit_amount = (double)gtk_spin_button_get_value(
      GTK_SPIN_BUTTON(depositData->deposit_amount));
  int month_term =
      (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(depositData->month_term));
  double interest_rate = (double)gtk_spin_button_get_value(
      GTK_SPIN_BUTTON(depositData->interest_rate));
  double tax_rate =
      (double)gtk_spin_button_get_value(GTK_SPIN_BUTTON(depositData->tax_rate));
  int repayment_periodicity = (int)gtk_spin_button_get_value(
      GTK_SPIN_BUTTON(depositData->repayment_periodicity));
  int interest_capitalization = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
                                    depositData->interest_capitalization))
                                    ? 1
                                    : 0;

  double accrued_interest;
  double tax_amount;
  double amount_on_deposit;

  int replenishment_count;
  double replenishment[1000][2];
  int withdraw_count;
  double withdraw[1000][2];

  // Считываем данные из depositData->tree_view_1
  GtkTreeIter iter;
  int i = 0;
  gboolean valid = gtk_tree_model_get_iter_first(
      GTK_TREE_MODEL(depositData->tree_store_1), &iter);
  while (valid) {
    gint month;
    gdouble amount;
    gtk_tree_model_get(GTK_TREE_MODEL(depositData->tree_store_1), &iter, 0,
                       &month, 1, &amount, -1);
    replenishment[i][1] = (double)month;
    replenishment[i][0] = amount;
    i++;
    valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(depositData->tree_store_1),
                                     &iter);
  }
  replenishment_count = i;

  i = 0;
  valid = gtk_tree_model_get_iter_first(
      GTK_TREE_MODEL(depositData->tree_store_2), &iter);
  while (valid) {
    gint month;
    gdouble amount;
    gtk_tree_model_get(GTK_TREE_MODEL(depositData->tree_store_2), &iter, 0,
                       &month, 1, &amount, -1);
    withdraw[i][1] = (double)month;
    withdraw[i][0] = amount;
    i++;
    valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(depositData->tree_store_2),
                                     &iter);
  }
  withdraw_count = i;

  int rc =
      calc_deposit(deposit_amount, month_term, interest_rate, tax_rate,
                   repayment_periodicity, interest_capitalization,
                   replenishment, replenishment_count, withdraw, withdraw_count,
                   &accrued_interest, &tax_amount, &amount_on_deposit);
  if (!rc) {
    gchar *accrued_interest_str =
        g_strdup_printf("Accrued Interest:\n%.2lf", accrued_interest);
    gchar *tax_amount_str = g_strdup_printf("Tax Amount:\n%.2lf", tax_amount);
    gchar *amount_on_deposit_str =
        g_strdup_printf("Amount on Deposit:\n%.2lf", amount_on_deposit);

    gtk_label_set_text(GTK_LABEL(depositData->accrued_interest),
                       accrued_interest_str);
    gtk_label_set_text(GTK_LABEL(depositData->tax_amount), tax_amount_str);
    gtk_label_set_text(GTK_LABEL(depositData->amount_on_deposit),
                       amount_on_deposit_str);

    g_free(accrued_interest_str);
    g_free(tax_amount_str);
    g_free(amount_on_deposit_str);
  }
}

void on_add_button_clicked_1(GtkButton *button, DepositData *depositData) {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Add replenishment", NULL, GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_ACCEPT,
      "Cancel", GTK_RESPONSE_CANCEL, NULL);
  GtkWidget *month_entry = gtk_entry_new();
  GtkWidget *amount_entry = gtk_entry_new();

  GtkWidget *month_label = gtk_label_new("Month:");
  GtkWidget *amount_label = gtk_label_new("Amount:");

  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_attach(GTK_GRID(grid), month_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), month_entry, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), amount_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), amount_entry, 1, 1, 1, 1);
  gtk_container_add(GTK_CONTAINER(content_area), grid);

  gtk_widget_show_all(dialog);

  gint response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response == GTK_RESPONSE_ACCEPT) {
    // Получаем введенные значения
    const char *month_text = gtk_entry_get_text(GTK_ENTRY(month_entry));
    const char *amount_text = gtk_entry_get_text(GTK_ENTRY(amount_entry));

    // Проверяем, что введенные значения корректны
    int month_value = atoi(month_text);
    double amount_value = atof(amount_text);

    if (month_value <= 0 || amount_value <= 0) {
      // Ошибка ввода, выводите сообщение об ошибке
      GtkWidget *error_dialog = gtk_message_dialog_new(
          NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
          "Invalid input. Month and amount must be positive values.");
      gtk_dialog_run(GTK_DIALOG(error_dialog));
      gtk_widget_destroy(error_dialog);
      gtk_widget_destroy(
          dialog);  // Закрываем окно только после закрытия сообщения об ошибке
      return;
    }
    GtkTreeStore *store = GTK_TREE_STORE(depositData->tree_store_1);
    GtkTreeIter iter;
    gtk_tree_store_append(store, &iter, NULL);
    gtk_tree_store_set(store, &iter, 0, month_value, 1, amount_value, -1);

    // Обновляем виджет GtkTreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(depositData->tree_view_1),
                            GTK_TREE_MODEL(store));

    // После добавления строки и обновления TreeView, может быть полезным
    // прокрутить TreeView, чтобы новая строка была видимой, если необходимо.
    GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(depositData->tree_view_1), path,
                                 NULL, TRUE, 0.5, 0.0);
    gtk_tree_path_free(path);
  }

  gtk_widget_destroy(dialog);
}

void on_add_button_clicked_2(GtkButton *button, DepositData *depositData) {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Add withdraw", NULL, GTK_DIALOG_MODAL, "Add", GTK_RESPONSE_ACCEPT,
      "Cancel", GTK_RESPONSE_CANCEL, NULL);
  // Создаем поля для ввода данных
  GtkWidget *month_entry = gtk_entry_new();
  GtkWidget *amount_entry = gtk_entry_new();

  // Создаем метки для полей
  GtkWidget *month_label = gtk_label_new("Month:");
  GtkWidget *amount_label = gtk_label_new("Amount:");

  // Создаем контейнеры для меток и полей
  GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *grid = gtk_grid_new();
  gtk_grid_attach(GTK_GRID(grid), month_label, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), month_entry, 1, 0, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), amount_label, 0, 1, 1, 1);
  gtk_grid_attach(GTK_GRID(grid), amount_entry, 1, 1, 1, 1);
  gtk_container_add(GTK_CONTAINER(content_area), grid);

  gtk_widget_show_all(dialog);

  gint response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response == GTK_RESPONSE_ACCEPT) {
    // Получаем введенные значения
    const char *month_text = gtk_entry_get_text(GTK_ENTRY(month_entry));
    const char *amount_text = gtk_entry_get_text(GTK_ENTRY(amount_entry));

    // Проверяем, что введенные значения корректны
    int month_value = atoi(month_text);
    double amount_value = atof(amount_text);

    if (month_value <= 0 || amount_value <= 0) {
      // Ошибка ввода, выводите сообщение об ошибке
      GtkWidget *error_dialog = gtk_message_dialog_new(
          NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
          "Invalid input. Month and amount must be positive values.");
      gtk_dialog_run(GTK_DIALOG(error_dialog));
      gtk_widget_destroy(error_dialog);
      gtk_widget_destroy(
          dialog);  // Закрываем окно только после закрытия сообщения об ошибке
      return;
    }

    GtkTreeStore *store = GTK_TREE_STORE(depositData->tree_store_2);

    GtkTreeIter iter;
    gtk_tree_store_append(store, &iter, NULL);

    gtk_tree_store_set(store, &iter, 0, month_value, 1, amount_value, -1);

    // Обновляем виджет GtkTreeView
    gtk_tree_view_set_model(GTK_TREE_VIEW(depositData->tree_view_2),
                            GTK_TREE_MODEL(store));

    // После добавления строки и обновления TreeView, может быть полезным
    // прокрутить TreeView, чтобы новая строка была видимой, если необходимо.
    GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(depositData->tree_view_2), path,
                                 NULL, TRUE, 0.5, 0.0);
    gtk_tree_path_free(path);
  }

  gtk_widget_destroy(dialog);
}

void on_rem_button_clicked_1(GtkButton *button, DepositData *depositData) {
  GtkTreeStore *store = GTK_TREE_STORE(depositData->tree_store_1);
  GtkTreeModel *model = GTK_TREE_MODEL(store);

  GtkTreeIter iter;
  GtkTreeIter last_iter;

  if (gtk_tree_model_get_iter_first(model, &iter)) {
    last_iter = iter;
    while (gtk_tree_model_iter_next(model, &iter)) {
      last_iter = iter;
    }
    gtk_tree_store_remove(store, &last_iter);
  }
}

void on_rem_button_clicked_2(GtkButton *button, DepositData *depositData) {
  GtkTreeStore *store = GTK_TREE_STORE(depositData->tree_store_2);
  GtkTreeModel *model = GTK_TREE_MODEL(store);

  GtkTreeIter iter;
  GtkTreeIter last_iter;

  if (gtk_tree_model_get_iter_first(model, &iter)) {
    last_iter = iter;
    while (gtk_tree_model_iter_next(model, &iter)) {
      last_iter = iter;
    }
    gtk_tree_store_remove(store, &last_iter);
  }
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

  gtk_init(&argc, &argv);

  GtkBuilder *builder;

  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "ui.glade", NULL);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Calculator");
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_widget_set_size_request(window, 800, 600);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  GtkWidget *notebook = gtk_notebook_new();
  gtk_container_add(GTK_CONTAINER(window), notebook);

  // Вкладка "Classic calculator with graph support" с калькулятором
  GtkWidget *calculator_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *calculator_label =
      gtk_label_new("Classic calculator with graph support");

  GtkWidget *grid1;
  grid1 = GTK_WIDGET(gtk_builder_get_object(builder, "main_grid"));
  gtk_container_add(GTK_CONTAINER(calculator_tab), grid1);

  //Кнопки

  GtkWidget *entr = GTK_WIDGET(gtk_builder_get_object(builder, "entr"));
  GtkWidget *entout = GTK_WIDGET(gtk_builder_get_object(builder, "entout"));
  GtkWidget *gspx = GTK_WIDGET(gtk_builder_get_object(builder, "gspx"));
  GtkWidget *l_edge = GTK_WIDGET(gtk_builder_get_object(builder, "gsp1"));
  GtkWidget *r_edge = GTK_WIDGET(gtk_builder_get_object(builder, "gsp2"));

  GtkWidget *b1 = GTK_WIDGET(gtk_builder_get_object(builder, "b1"));
  GtkWidget *b2 = GTK_WIDGET(gtk_builder_get_object(builder, "b2"));
  GtkWidget *b3 = GTK_WIDGET(gtk_builder_get_object(builder, "b3"));
  GtkWidget *b4 = GTK_WIDGET(gtk_builder_get_object(builder, "b4"));
  GtkWidget *b5 = GTK_WIDGET(gtk_builder_get_object(builder, "b5"));
  GtkWidget *b6 = GTK_WIDGET(gtk_builder_get_object(builder, "b6"));
  GtkWidget *b7 = GTK_WIDGET(gtk_builder_get_object(builder, "b7"));
  GtkWidget *b8 = GTK_WIDGET(gtk_builder_get_object(builder, "b8"));
  GtkWidget *b9 = GTK_WIDGET(gtk_builder_get_object(builder, "b9"));
  GtkWidget *b0 = GTK_WIDGET(gtk_builder_get_object(builder, "b0"));
  GtkWidget *bd = GTK_WIDGET(gtk_builder_get_object(builder, "bd"));
  GtkWidget *bs1 = GTK_WIDGET(gtk_builder_get_object(builder, "bs1"));
  GtkWidget *bs2 = GTK_WIDGET(gtk_builder_get_object(builder, "bs2"));
  GtkWidget *bsin = GTK_WIDGET(gtk_builder_get_object(builder, "bsin"));
  GtkWidget *bcos = GTK_WIDGET(gtk_builder_get_object(builder, "bcos"));
  GtkWidget *basin = GTK_WIDGET(gtk_builder_get_object(builder, "basin"));
  GtkWidget *bacos = GTK_WIDGET(gtk_builder_get_object(builder, "bacos"));
  GtkWidget *btan = GTK_WIDGET(gtk_builder_get_object(builder, "btan"));
  GtkWidget *batan = GTK_WIDGET(gtk_builder_get_object(builder, "batan"));
  GtkWidget *bln = GTK_WIDGET(gtk_builder_get_object(builder, "bln"));
  GtkWidget *blog = GTK_WIDGET(gtk_builder_get_object(builder, "blog"));
  GtkWidget *bmod = GTK_WIDGET(gtk_builder_get_object(builder, "bmod"));
  GtkWidget *bsqrt = GTK_WIDGET(gtk_builder_get_object(builder, "bsqrt"));
  GtkWidget *bp = GTK_WIDGET(gtk_builder_get_object(builder, "bp"));
  GtkWidget *bm = GTK_WIDGET(gtk_builder_get_object(builder, "bm"));
  GtkWidget *bdiv = GTK_WIDGET(gtk_builder_get_object(builder, "bdiv"));
  GtkWidget *bmu = GTK_WIDGET(gtk_builder_get_object(builder, "bmu"));
  GtkWidget *bx = GTK_WIDGET(gtk_builder_get_object(builder, "bx"));
  GtkWidget *bpo = GTK_WIDGET(gtk_builder_get_object(builder, "bpo"));

  g_signal_connect(b1, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b2, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b3, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b4, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b5, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b6, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b7, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b8, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b9, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(b0, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bd, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bm, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bmu, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bp, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bpo, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bsin, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(basin, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bcos, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bacos, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(btan, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(batan, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(blog, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bln, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bx, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bsqrt, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bmod, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bs1, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bs2, "clicked", G_CALLBACK(button_print), entr);
  g_signal_connect(bdiv, "clicked", G_CALLBACK(button_print), entr);

  GtkWidget *bc = GTK_WIDGET(gtk_builder_get_object(builder, "bc"));
  g_signal_connect(bc, "clicked", G_CALLBACK(button_clear), entr);

  GtkWidget *br = GTK_WIDGET(gtk_builder_get_object(builder, "br"));
  UserData *userData = g_malloc(sizeof(UserData));
  userData->entry = entr;
  userData->output = entout;
  userData->x_field = gspx;
  userData->l_edge = l_edge;
  userData->r_edge = r_edge;
  g_signal_connect(br, "clicked", G_CALLBACK(button_calc_expr), userData);

  // График

  GtkWidget *area = GTK_WIDGET(gtk_builder_get_object(builder, "graph"));
  GtkWidget *bgr = GTK_WIDGET(gtk_builder_get_object(builder, "bgr"));
  gtk_widget_set_redraw_on_allocate(area, FALSE);

  PlotData *plotData = g_malloc(sizeof(PlotData));
  plotData->entry = entr;
  plotData->area = area;
  g_signal_connect(bgr, "clicked", G_CALLBACK(go_flag), userData);
  g_signal_connect(bgr, "clicked", G_CALLBACK(button_draw_graph), plotData);
  g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_callback), userData);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), calculator_tab,
                           calculator_label);

  // Вкладка "Credit calculator"
  GtkWidget *credit_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *credit_label = gtk_label_new("Credit calculator");

  GtkWidget *grid2;
  grid2 = GTK_WIDGET(gtk_builder_get_object(builder, "credit_grid"));
  gtk_container_add(GTK_CONTAINER(credit_tab), grid2);

  GtkWidget *spc1 = GTK_WIDGET(gtk_builder_get_object(builder, "spc1"));
  GtkWidget *spc2 = GTK_WIDGET(gtk_builder_get_object(builder, "spc2"));
  GtkWidget *spc3 = GTK_WIDGET(gtk_builder_get_object(builder, "spc3"));
  GtkWidget *rb1 = GTK_WIDGET(gtk_builder_get_object(builder, "rb1"));
  GtkWidget *ce1 = GTK_WIDGET(gtk_builder_get_object(builder, "ce1"));
  GtkWidget *ce2 = GTK_WIDGET(gtk_builder_get_object(builder, "ce2"));
  GtkWidget *tree = GTK_WIDGET(gtk_builder_get_object(builder, "tree"));

  CreditData *creditData = g_malloc(sizeof(CreditData));
  creditData->overpayment_field = ce1;
  creditData->total_payment_field = ce2;
  creditData->tree_view = tree;
  creditData->total_loan_amount = spc1;
  creditData->month_term = spc2;
  creditData->interest_rate = spc3;
  creditData->type = rb1;

  // Добавление столбца "Month" в TreeView
  GtkTreeViewColumn *month_column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(month_column, "Month");
  GtkCellRenderer *month_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(month_column, month_renderer, TRUE);
  gtk_tree_view_column_add_attribute(month_column, month_renderer, "text", 0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(creditData->tree_view),
                              month_column);

  // Добавление столбца "Amount" в TreeView
  GtkTreeViewColumn *amount_column = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(amount_column, "Amount");
  GtkCellRenderer *amount_renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(amount_column, amount_renderer, TRUE);
  gtk_tree_view_column_add_attribute(amount_column, amount_renderer, "text", 1);
  gtk_tree_view_append_column(GTK_TREE_VIEW(creditData->tree_view),
                              amount_column);

  // Подключение обработчика к кнопке
  GtkWidget *bcc = GTK_WIDGET(gtk_builder_get_object(builder, "bcc"));
  g_signal_connect(bcc, "clicked", G_CALLBACK(button_calc_credit), creditData);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), credit_tab, credit_label);

  // Вкладка "Deposit calculator"
  GtkWidget *deposit_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *deposit_label = gtk_label_new("Deposit calculator");

  GtkWidget *grid3;
  grid3 = GTK_WIDGET(gtk_builder_get_object(builder, "deposit_grid"));
  gtk_container_add(GTK_CONTAINER(deposit_tab), grid3);

  GtkWidget *spd1 = GTK_WIDGET(gtk_builder_get_object(builder, "spd1"));
  GtkWidget *spd2 = GTK_WIDGET(gtk_builder_get_object(builder, "spd2"));
  GtkWidget *spd3 = GTK_WIDGET(gtk_builder_get_object(builder, "spd3"));
  GtkWidget *spd4 = GTK_WIDGET(gtk_builder_get_object(builder, "spd4"));
  GtkWidget *spd5 = GTK_WIDGET(gtk_builder_get_object(builder, "spd5"));
  GtkWidget *chkb = GTK_WIDGET(gtk_builder_get_object(builder, "chkb"));
  GtkWidget *de1 = GTK_WIDGET(gtk_builder_get_object(builder, "de1"));
  GtkWidget *de2 = GTK_WIDGET(gtk_builder_get_object(builder, "de2"));
  GtkWidget *de3 = GTK_WIDGET(gtk_builder_get_object(builder, "de3"));
  GtkWidget *treed1 = GTK_WIDGET(gtk_builder_get_object(builder, "treed1"));
  GtkWidget *treed2 = GTK_WIDGET(gtk_builder_get_object(builder, "treed2"));

  DepositData *depositData = g_malloc(sizeof(DepositData));
  depositData->accrued_interest = de1;
  depositData->tax_amount = de2;
  depositData->amount_on_deposit = de3;
  depositData->tree_view_1 = treed1;
  depositData->tree_view_2 = treed2;
  depositData->deposit_amount = spd1;
  depositData->month_term = spd2;
  depositData->interest_rate = spd3;
  depositData->tax_rate = spd4;
  depositData->repayment_periodicity = spd5;
  depositData->interest_capitalization = chkb;
  depositData->tree_store_1 = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_DOUBLE);
  depositData->tree_store_2 = gtk_tree_store_new(2, G_TYPE_INT, G_TYPE_DOUBLE);

  // Добавление столбца "Month" в TreeView
  GtkTreeViewColumn *month_column_1 = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(month_column_1, "Month");
  GtkCellRenderer *month_renderer_1 = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(month_column_1, month_renderer_1, TRUE);
  gtk_tree_view_column_add_attribute(month_column_1, month_renderer_1, "text",
                                     0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(depositData->tree_view_1),
                              month_column_1);

  // Добавление столбца "Amount" в TreeView
  GtkTreeViewColumn *amount_column_1 = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(amount_column_1, "Amount");
  GtkCellRenderer *amount_renderer_1 = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(amount_column_1, amount_renderer_1, TRUE);
  gtk_tree_view_column_add_attribute(amount_column_1, amount_renderer_1, "text",
                                     1);
  gtk_tree_view_append_column(GTK_TREE_VIEW(depositData->tree_view_1),
                              amount_column_1);

  // Добавление столбца "Month" в TreeView
  GtkTreeViewColumn *month_column_2 = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(month_column_2, "Month");
  GtkCellRenderer *month_renderer_2 = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(month_column_2, month_renderer_2, TRUE);
  gtk_tree_view_column_add_attribute(month_column_2, month_renderer_2, "text",
                                     0);
  gtk_tree_view_append_column(GTK_TREE_VIEW(depositData->tree_view_2),
                              month_column_2);

  // Добавление столбца "Amount" в TreeView
  GtkTreeViewColumn *amount_column_2 = gtk_tree_view_column_new();
  gtk_tree_view_column_set_title(amount_column_2, "Amount");
  GtkCellRenderer *amount_renderer_2 = gtk_cell_renderer_text_new();
  gtk_tree_view_column_pack_start(amount_column_2, amount_renderer_2, TRUE);
  gtk_tree_view_column_add_attribute(amount_column_2, amount_renderer_2, "text",
                                     1);
  gtk_tree_view_append_column(GTK_TREE_VIEW(depositData->tree_view_2),
                              amount_column_2);

  // Подключение обработчика к кнопке
  GtkWidget *bcd = GTK_WIDGET(gtk_builder_get_object(builder, "bcd"));
  g_signal_connect(bcd, "clicked", G_CALLBACK(button_calc_deposit),
                   depositData);

  GtkWidget *addb1 = GTK_WIDGET(gtk_builder_get_object(builder, "addb1"));
  GtkWidget *addb2 = GTK_WIDGET(gtk_builder_get_object(builder, "addb2"));

  g_signal_connect(addb1, "clicked", G_CALLBACK(on_add_button_clicked_1),
                   depositData);
  g_signal_connect(addb2, "clicked", G_CALLBACK(on_add_button_clicked_2),
                   depositData);

  GtkWidget *brem1 = GTK_WIDGET(gtk_builder_get_object(builder, "brem1"));
  GtkWidget *brem2 = GTK_WIDGET(gtk_builder_get_object(builder, "brem2"));

  g_signal_connect(brem1, "clicked", G_CALLBACK(on_rem_button_clicked_1),
                   depositData);
  g_signal_connect(brem2, "clicked", G_CALLBACK(on_rem_button_clicked_2),
                   depositData);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), deposit_tab, deposit_label);

  gtk_widget_show_all(window);
  gtk_main();

  return 0;
}
