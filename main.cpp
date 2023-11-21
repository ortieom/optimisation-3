#include <iostream>
#include<vector>
#include<climits>

using namespace std;

struct vector_node {
  int row;
  int column;
  double amount;
  vector_node() {
    row = -1;
    column = -1;
    amount = -1;
  }
  vector_node(int r, int c, double a) {
    row = r;
    column = c;
    amount = a;
  }
};

void print_table(const vector < vector < double >> & table) {
  // Print header row
  cout << "\n\t\tDestinations\n";
  cout << "\t\t";
  for (int j = 1; j <= 4; ++j) {
    cout << "D" << j << "\t";
  }
  cout << "Supply\n";

  // Print table rows
  for (int i = 1; i <= 3; ++i) {
    cout << "Source " << i << ":\t";
    for (int j = 1; j <= 4; ++j) {
      cout << table[i][j] << "\t";
    }
    cout << table[i][0] << "\n";
  }

  // Print demand row
  cout << "Demand\t\t";
  for (int j = 1; j <= 4; ++j) {
    cout << table[0][j] << "\t";
  }
  cout << "\n\n";
}

vector < vector_node > north_west(vector < vector < double >> table) {
  vector < vector_node > answer;
  int pos_col = 1, pos_row = 1;
  // detecting if all supply was distributed to destinations
  while (table[0][1] + table[0][2] + table[0][3] + table[0][4] > 0) {
    // selecting the amount of supply to give to the destination
    double value = min(table[pos_row][0], table[0][pos_col]);
    answer.emplace_back(pos_row, pos_col, value);
    table[pos_row][0] -= value;
    table[0][pos_col] -= value;
    // if supply is finished go to the next supply
    if (table[pos_row][0] == 0) {
      pos_row++;
    }
    // if destination is fulfilled go to the next destination
    if (table[0][pos_col] == 0) {
      pos_col++;
    }
  }
  return answer;
}

// function to find two minimum values in a vector
pair < double, double > find_two_min_values(vector < double > & active_elements) {
  double min_one = max(active_elements[0], active_elements[1]);
  double min_two = min(active_elements[0], active_elements[1]);
  for (int l = 2; l < active_elements.size(); ++l) {
    if (active_elements[l] < min_two) {
      min_one = min_two;
      min_two = active_elements[l];
    } else if (active_elements[l] < min_one) {
      min_one = active_elements[l];
    }
  }
  return make_pair(min_one, min_two);
}

// function to find the largest value and its index in a vector
pair < int, double > find_max_in_vector(vector < double > & vec) {
  double mx = -1;
  int max_index = -1;
  for (int l = 0; l < vec.size(); ++l) {
    if (vec[l] > mx) {
      mx = vec[l];
      max_index = l;
    }
  }
  return make_pair(max_index, mx);
}

// Vogel's approximation method
vector < vector_node > vogels(vector < vector < double >> table) {
  vector < vector_node > answer;
  //vectors to keep row differents and column differents between the least values
  vector < double > row_diff(3);
  vector < double > column_diff(4);
  // loop until all destinations got needed supply
  while (table[0][1] + table[0][2] + table[0][3] + table[0][4] > 0) {
    for (int j = 1; j < 5; ++j) {
      // if destination doesn't need any supply anymore the column is inactive
      if (table[0][j] == 0) {
        column_diff[j - 1] = -1;
        continue;
      }
      vector < double > active_elements;
      for (int i = 1; i < 4; ++i) {
        // if supply is empty do not consider the cost from that source
        if (table[i][0] == 0) {
          continue;
        }
        active_elements.push_back(table[i][j]);
      }
      if (active_elements.size() == 1) {
        column_diff[j - 1] = 0;
      } else {
        //make difference of jth column to be the difference between two least values
        auto two_min_values = find_two_min_values(active_elements);
        column_diff[j - 1] = two_min_values.first - two_min_values.second;
      }
    }

    for (int i = 1; i < 4; ++i) {
      //do not consider the source if it is empty
      if (table[i][0] == 0) {
        row_diff[i - 1] = -1;
        continue;
      }
      vector < double > active_elements;
      for (int j = 1; j < 5; ++j) {
        //consider only destinations with demand
        if (table[0][j] == 0) {
          continue;
        }
        active_elements.push_back(table[i][j]);
      }
      if (active_elements.size() == 1) {
        row_diff[i - 1] = 0;
      } else {
        //make row difference as the difference between two least elements in a row
        auto two_min_values = find_two_min_values(active_elements);
        row_diff[i - 1] = two_min_values.first - two_min_values.second;
      }
    }
    //finding maximal difference in rows differences and columns differences separately
    auto found_in_row_diff = find_max_in_vector(row_diff);
    double max_in_row_diff = found_in_row_diff.second;
    int max_in_row_diff_index = found_in_row_diff.first;
    auto found_in_column_diff = find_max_in_vector(column_diff);
    double max_in_column_diff = found_in_column_diff.second;
    int max_in_column_diff_index = found_in_column_diff.first;
    //we need to choose the maximal difference
    if (max_in_column_diff >= max_in_row_diff) {
      //finding the minimal cost in the column with maximal column difference
      int min_cost_row_index = -1;
      double min_cost = -1;
      for (int i = 1; i < 4; ++i) {
        if (table[i][0] == 0) {
          continue;
        }
        if (min_cost == -1 || min_cost > table[i][max_in_column_diff_index + 1]) {
          min_cost = table[i][max_in_column_diff_index + 1];
          min_cost_row_index = i;
        }
      }
      //updating the table
      double value = min(table[min_cost_row_index][0], table[0][max_in_column_diff_index + 1]);
      table[min_cost_row_index][0] -= value;
      table[0][max_in_column_diff_index + 1] -= value;
      answer.emplace_back(min_cost_row_index, max_in_column_diff_index + 1, value);
    } else {
      //choosing the minimal element in a row with maximal row difference
      int min_cost_column_index = -1;
      double min_cost = -1;
      for (int j = 1; j < 5; ++j) {
        if (table[0][j] == 0) {
          continue;
        }
        if (min_cost == -1 || min_cost > table[max_in_row_diff_index + 1][j]) {
          min_cost = table[max_in_row_diff_index + 1][j];
          min_cost_column_index = j;
        }
      }
      //updating the table
      double value = min(table[0][min_cost_column_index], table[max_in_row_diff_index + 1][0]);
      table[0][min_cost_column_index] -= value;
      table[max_in_row_diff_index + 1][0] -= value;
      answer.emplace_back(max_in_row_diff_index + 1, min_cost_column_index, value);
    }

  }
  return answer;
}

vector < vector_node > russels(vector < vector < double >> table) {
  vector < vector_node > answer;
  while (table[0][1] + table[0][2] + table[0][3] + table[0][4] > 0) {
    //vectors for largest items in rows and columns respectively
    vector < vector_node > u(3);
    vector < vector_node > v(4);
    //calculating u
    for (int i = 1; i < 4; ++i) {
      if (table[i][0] == 0) {
        u[i - 1] = vector_node(-1, -1, -1);
        continue;
      }
      int ind_of_max_el_in_column = -1;
      double max_el;
      for (int j = 1; j < 5; ++j) {
        if (table[0][j] == 0) {
          continue;
        }
        if (ind_of_max_el_in_column == -1 || table[i][j] > max_el) {
          max_el = table[i][j];
          ind_of_max_el_in_column = j;
        }
      }
      u[i - 1] = vector_node(i, ind_of_max_el_in_column, table[i][ind_of_max_el_in_column]);
    }

    //calculating v
    for (int j = 1; j < 5; ++j) {
      if (table[0][j] == 0) {
        v[j - 1] = vector_node(-1, -1, -1);
        continue;
      }
      int ind_of_max_el_in_row = -1;
      double max_el;
      for (int i = 1; i < 4; ++i) {
        if (table[i][0] == 0) {
          continue;
        }
        if (ind_of_max_el_in_row == -1 || table[i][j] > max_el) {
          max_el = table[i][j];
          ind_of_max_el_in_row = i;
        }
      }
      v[j - 1] = vector_node(ind_of_max_el_in_row, j, table[ind_of_max_el_in_row][j]);
    }
    //finding deltas for cells in active columns and rows that was not used on previous step
    vector < vector_node > deltas;
    for (int i = 1; i < 4; ++i) {
      for (int j = 1; j < 5; ++j) {
        if (table[0][j] == 0 || table[i][0] == 0) {
          continue;
        }
        if (j != u[i - 1].column && i != v[j - 1].row) {
          deltas.emplace_back(i, j, table[i][j] - u[i - 1].amount - v[j - 1].amount);
        }
      }
    }
    //if deltas empty implies that we need to finish
    if (deltas.empty()) {
      for (auto & u_temp: u) {
        if (u_temp.column == -1 || u_temp.row == -1) {
          continue;
        }
        double value = min(table[0][u_temp.column], table[u_temp.row][0]);
        if (value == 0) {
          continue;
        }
        table[0][u_temp.column] -= value;
        table[u_temp.row][0] -= value;
        answer.emplace_back(u_temp.row, u_temp.column, value);
      }
      for (auto & v_temp: v) {
        if (v_temp.column == -1 || v_temp.row == -1) {
          continue;
        }
        double value = min(table[0][v_temp.column], table[v_temp.row][0]);
        if (value == 0) {
          continue;
        }
        table[0][v_temp.column] -= value;
        table[v_temp.row][0] -= value;
        answer.emplace_back(v_temp.row, v_temp.column, value);
      }
    } else {
      //choosing the most negative delta
      vector_node min_value = deltas[0];
      for (int i = 1; i < deltas.size(); ++i) {
        if (deltas[i].amount < min_value.amount) {
          min_value = deltas[i];
        }
      }
      //updating the table
      double value = min(table[min_value.row][0], table[0][min_value.column]);
      table[min_value.row][0] -= value;
      table[0][min_value.column] -= value;
      answer.emplace_back(min_value.row, min_value.column, value);
    }
  }
  return answer;
}

int main() {
  //first row is destination demand. first column is supply
  vector < vector < double >> table(4, vector < double > (5));
  double total_demand = 0.0, total_supply = 0.0;
  cout << "Enter supply amount for each of 3 sources in next form s1 s2 s3:\n";
  for (int i = 1; i < 4; ++i) {
    cin >> table[i][0];
    total_supply += table[i][0];
  }

  cout << "Enter demand amount for each of 4 destinations in next form d1 d2 d3 d4:\n";
  for (int i = 1; i < 5; ++i) {
    cin >> table[0][i];
    total_demand += table[0][i];
  }

  if (total_supply != total_demand) {
    cout << "The problem is not balanced!\n";
    return 0;
  }

  cout << "Enter matrix of costs like c1 c2 c3...\n";
  for (int i = 1; i < 4; ++i) {
    for (int j = 1; j < 5; ++j) {
      cin >> table[i][j];
      if (table[i][j] < 0) {
        cout << "The method is not applicable!\n";
        return 0;
      }
    }
  }

  print_table(table);
  auto answer = north_west(table);
  cout << "Initial solution by North-West corner method\n";
  for (auto & c: answer) {
    cout << "x" << c.row << c.column << " = " << c.amount << endl;
  }

  answer = vogels(table);
  cout << "Initial solution by Vogel's approximation method\n";
  for (auto & c: answer) {
    cout << "x" << c.row << c.column << " = " << c.amount << endl;
  }

  answer = russels(table);
  cout << "Initial solution by Russel's approximation method\n";
  for (auto & c: answer) {
    cout << "x" << c.row << c.column << " = " << c.amount << endl;
  }

  return 0;
}
