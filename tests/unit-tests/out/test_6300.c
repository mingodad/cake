
char * /*_Owner*/ strdup(const char* s);
void free(void* /*_Owner*/ p);

struct X {
  char * /*_Owner*/ text;
};

void f(int a)
{
    struct X x = {0};
    x.text = strdup("a");
    free(x.text);
}