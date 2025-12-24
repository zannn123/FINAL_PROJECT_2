#include "globals.h"
#include "file_handler.h"
#include "ui_core.h"
#include <iostream>

using namespace std;

int main() {

    showLoadingView("HOW COULD GIVE ME SOME SIGN THAT LEAD TO WHAT WE HAVE? ");
    showLoginView();

    cin.ignore();
    cin.get();
    return 0;
}