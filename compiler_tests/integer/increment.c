int f() {
    int a = 5;
    int b = 10;
    int c = 20;

    return  (a == 5) &&
            (a++ == 5) &&
            (b == b++) &&
            (a-- != a) &&
            (1+a == ++a) &&
            (a-1 == --a) &&
            (c+1 != c++) &&
            (c == 21);
}