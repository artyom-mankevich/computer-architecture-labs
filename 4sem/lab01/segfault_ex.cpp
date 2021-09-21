int main() {
    int a[20];
    for (int i = 0; i < sizeof(a)/sizeof(*a); i++) {
        a[i] = i;
    }
}
