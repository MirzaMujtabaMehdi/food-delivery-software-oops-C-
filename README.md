# Food Ordering Software – Object-Oriented Version (C++)

> **Academic notice:** This project was written for learning and academic purposes only. It is an exercise in object-oriented programming (abstraction, inheritance, polymorphism, encapsulation, friend functions, static members) and file handling. It is not a real ordering system.

---

## 1. Overview

An object-oriented rewrite of the procedural *Food delivery software*. It has a customer side (view menu, place an order, search order history) and an **admin panel** (add an item, remove an item, restock). The menu, prices and stock live in memory and are shared by every part of the program.

| Menu choice | Handled by class | What it does |
|---|---|---|
| 1 Show menu | `menuservice` | Lists items, prices and stock left |
| 2 Order food | `orderservice` | Takes customer details and dishes, reduces stock, saves and prints the bill |
| 3 Show history | `historyservice` | Searches `orders.txt` by mobile number |
| 4 Admin panel | `adminservice` | Add / remove / restock items |
| 5 or any other key | – | Exit |

## 2. Files

| File | Purpose |
|---|---|
| `MAIN2.CPP` | Complete source code |
| `MAIN2.exe` | Windows executable built from the source |

`orders.txt` is created next to the program when the first order is placed.

## 3. Build and run

```bash
g++ -std=c++11 MAIN2.CPP -o main2
./main2         # Windows: main2.exe
```

(C++11 or later is needed for `override` and `nullptr`.)

## 4. Class design

```
Restaurant  (abstract: virtual void execute() = 0)
 ├── menuservice      has-a  menu
 ├── orderservice     has-a  order
 ├── historyservice   has-a  search_history
 └── adminservice     has-a  Admin

menu   (static vectors: prices, items, stock;  showmenu())
 ├── order   (takeorder, slip_calculation; has customerinfo, filesaving, search_history; friend printbill)
 └── Admin   (add_remove_item)

customerinfo    – name, address, phone with getters
filesaving      – saveorder(...) appends to orders.txt
search_history  – searchorder() reads orders.txt
```

## 5. OOP concepts demonstrated

| Concept | Where | Why it is used |
|---|---|---|
| **Abstraction** – abstract class with a pure virtual function | `Restaurant::execute() = 0` | Defines one common action ("do your job") that every service must provide |
| **Inheritance** | `order` and `Admin` inherit `menu`; four services inherit `Restaurant` | Reuse `showmenu()` and the shared data; treat services uniformly |
| **Polymorphism** (runtime) | `Restaurant* R; R = new orderservice; R->execute();` | `main()` has one pointer type; the correct `execute()` runs depending on the object created, so no long `if/else` of function calls is needed |
| **Encapsulation** | `private` members in `customerinfo`, `order`, `Admin`; public getters | Data is accessed only through methods |
| **Static data members** | `menu::prices`, `menu::items`, `menu::stock` | Every object (customer order *and* admin) sees the same menu and stock; an admin change is instantly visible to orders |
| **Friend function** | `friend void printbill(order)` | Lets a non-member function read the private `slip` and `total` |
| **Composition** | `order` contains `customerinfo`, `filesaving`, `search_history` | Splits responsibilities into small classes |
| **Constructor + initializer list** | `order():total(0),dish_no(0),…{}` | Gives members safe starting values |

## 6. Functions

### 6.1 User-defined (by class)

| Class | Member | Purpose |
|---|---|---|
| `menu` | `showmenu()` | Prints `index. item : price : stock left` for every entry |
| `customerinfo` | constructor, `getdata()`, `getname()`, `getaddress()`, `getphone_no()` | Reads and exposes customer details (`cin.ignore()` + `getline` for name/address) |
| `filesaving` | `saveorder(name, phone, address, slip, tamount)` | Appends one line to `orders.txt` if the file opens |
| `search_history` | `searchorder()` | Reads a number as text and prints each `orders.txt` line containing it |
| `order` | `displaymenu()`, `takeorder()`, `slip_calculation(quantity, index)` | Whole ordering flow (see 7); `slip_calculation` adds `quantity × price[index]` to `total` and writes the line text into `slip` |
| `Admin` | `add_remove_item()` | 1 = insert an item at a chosen position, 2 = erase an item, 3 = add stock |
| `orderservice`, `menuservice`, `historyservice`, `adminservice` | `execute() override` | Each calls the matching class |
| (free function) | `printbill(order o)` | Prints the slip and total |
| `main()` | – | Menu loop that creates the chosen service object and calls `execute()` |

### 6.2 Built-in / library features

| Feature | Header | Used for | Why |
|---|---|---|---|
| `vector<float>`, `vector<string>`, `vector<int>` | `<vector>` | Prices, item names, stock | Resizable, so items can be added/removed at run time |
| `.size()`, `.insert(begin()+i, v)`, `.erase(begin()+i)`, `.push_back(v)` | `<vector>` | Admin operations | Keep three parallel lists in step |
| `ofstream(...,ios::app)`, `ifstream`, `.is_open()` | `<fstream>` | Save / read orders | `app` keeps the old orders; `is_open` guards against a failed open |
| `getline`, `cin.ignore`, `to_string`, `line.find`, `string::npos` | `<string>`, `<iostream>` | Input, bill text, history search | Same roles as in the procedural version |
| `new`, `delete`, `nullptr` | language | Create service objects dynamically | Needed for runtime polymorphism through a base pointer |
| `virtual`, `override` | language | Polymorphism | `override` makes the compiler check the signature matches |
| `static` | language | Shared data | One copy of the menu for the whole program |

## 7. Step-by-step logic

**Ordering (`order::takeorder`)**
1. `c.getdata()` asks for name, address, phone.
2. Loop: show the menu → read dish number → convert to index (`dish_no - 1`) → if the index is inside the list, read quantity.
3. If `stock[index] >= quantity`, subtract it from stock and add the line to the slip; otherwise print "insufficient stock".
4. Ask "do you want more"; repeat while the answer is `y`/`Y`.
5. Save the order with `filesaving::saveorder(...)` and print the bill with the friend function `printbill(*this)`.

**Why a new service object per request?** `main()` runs `R = new orderservice;` each time, so every order starts with a fresh `total` and `slip`, while the *menu and stock* survive because they are static.

**Admin (`Admin::add_remove_item`)** – the three vectors are parallel (index *i* of each describes one dish), so every insert/erase is applied to all three.

## 8. Default data

8 items: Biryani 350, Nihari 200, Karahi 400, pizza Large 500, Pizza Regular 300, Pizza Small 200, Burger 250, Broast 320, each with a starting stock count. Menu and stock changes are **not saved**; they reset when the program restarts.

## 9. Known limitations (checked by running the program)

1. **Double billing.** `takeorder()` calls `slip_calculation()` once *before* the stock check and again *inside* it, so an order of 2 Burgers at 250 produced a total of **1000** instead of 500. Remove the first call.
2. **Out-of-stock items are still billed.** Because of the first call above, ordering 10 Burgers when only 6 exist prints "insufficient stock" but still bills and saves 10.
3. **Memory management.** A new object is created every loop with `new`, but only the last one is `delete`d, and `Restaurant` has no `virtual` destructor (deleting through a base pointer is undefined without it). Using `std::unique_ptr` or a virtual destructor would fix it.
4. **Admin remove bounds check** uses `<=` instead of `<`, so entering `size+1` erases past the end of the vectors.
5. The admin prompt mentions only options 1 and 2, but option 3 (restock) exists.
6. Item names are read with `cin >>`, so they cannot contain spaces.
7. `printbill` takes the `order` object **by value** (a full copy); passing by reference would be cheaper.
8. Order history search is a substring search on the whole line.

## 10. Ideas for improvement

Fix the double call; use `unique_ptr`; save the menu to a file; validate quantities; use a `struct Item { string name; float price; int stock; }` instead of three parallel vectors.
