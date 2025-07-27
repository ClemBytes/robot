#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NOT_A_POINT_VIRGULE ;

void main(void) {
    unsigned char entier_non_signe_8bits = 42;
    printf("Entier non signé 8 bits de valeur 42: 0b%08b (binaire) | 0x%02x (hexa)\n", entier_non_signe_8bits, entier_non_signe_8bits);
    signed char entier_signe_8bits = 42;
    printf("Entier signé 8 bits de valeur 42:     0b%08b (binaire) | 0x%02x (hexa)\n", entier_signe_8bits, entier_signe_8bits);
    signed char entier_signe_8bits_negatif = -42;
    printf("Entier signé 8 bits de valeur -42:    0b%08b (binaire) | 0x%02x (hexa)\n", (unsigned char) entier_signe_8bits_negatif, (unsigned char) entier_signe_8bits_negatif);

    printf("\n");

    unsigned short entier_non_signe_16bits = 342;
    printf("Entier non signé 16 bits de valeur 342: 0b%016b (binaire) | 0x%04x (hexa)\n", entier_non_signe_16bits, entier_non_signe_16bits);
    signed short entier_signe_16bits = 342;
    printf("Entier signé 16 bits de valeur 342:     0b%016b (binaire) | 0x%04x (hexa)\n", entier_signe_16bits, entier_signe_16bits);
    signed short entier_signe_16bits_negatif = -342;
    printf("Entier signé 16 bits de valeur -342:    0b%016b (binaire) | 0x%04x (hexa)\n", (unsigned short) entier_signe_16bits_negatif, (unsigned short) entier_signe_16bits_negatif);

    unsigned short* pointer_to_u16 = &entier_non_signe_16bits;
    unsigned char* pointer_to_bytes_of_u16 = (unsigned char*) pointer_to_u16;
    printf("Bytes of the u16: %02x %02x\n", pointer_to_bytes_of_u16[0], pointer_to_bytes_of_u16[1]);

    printf("\n");

    unsigned int entier_non_signe_32bits = 65542;
    printf("Entier non signé 32 bits de valeur 65 542: 0b%032b (binaire) | 0x%08x (hexa)\n", entier_non_signe_32bits, entier_non_signe_32bits);
    signed int entier_signe_32bits = 65542;
    printf("Entier signé 32 bits de valeur 65 542:     0b%032b (binaire) | 0x%08x (hexa)\n", entier_signe_32bits, entier_signe_32bits);
    signed int entier_signe_32bits_negatif = -65542;
    printf("Entier signé 32 bits de valeur -65 542:    0b%032b (binaire) | 0x%08x (hexa)\n", (unsigned int) entier_signe_32bits_negatif, (unsigned int) entier_signe_32bits_negatif);

    unsigned int* pointer_to_u32 = &entier_non_signe_32bits;
    unsigned char* pointer_to_bytes_of_u32 = (unsigned char*) pointer_to_u32;
    printf("Bytes of the u32: %02x %02x %02x %02x\n", pointer_to_bytes_of_u32[0], pointer_to_bytes_of_u32[1], pointer_to_bytes_of_u32[2], pointer_to_bytes_of_u32[3]);

    printf("\n");

    unsigned long entier_non_signe_64bits = 756123456789;
    unsigned long* pointer_to_u64 = &entier_non_signe_64bits;
    unsigned char* pointer_to_bytes_of_u64 = (unsigned char*) pointer_to_u64;
    printf("Bytes of the u64: ");
    for (size_t i = 0; i < 8; i++) {
        printf("%02x ", pointer_to_bytes_of_u64[i]);
    }
    printf("\n");

    float flottant_simple = 42.0;
    printf("Flottant simple précision de valeur 42.0: %f (flottant) | %a (hex float)\n", flottant_simple, flottant_simple);
    float* pointer_to_f32 = &flottant_simple;
    unsigned char* pointer_to_bytes_of_f32 = (unsigned char*) pointer_to_f32;
    printf("Bytes of the f32: ");
    for (size_t i = 0; i < 4; i++) {
        printf("%02x ", pointer_to_bytes_of_f32[i]);
    }
    printf("\n");

    printf("\n");

    struct personne{
        char* nom;
        char* prenom;
        char favorite_letter;
        unsigned int age;
        float taille;
    };

    struct personne misty =  (struct personne)
    {
        .nom = "Chaton",
        .prenom = "Misty",
        .favorite_letter = 'c',
        .age = 9,
        .taille = 0.42,
    };

    struct personne* pointer_to_misty = &misty;
    unsigned char* pointer_to_bytes_of_misty = (unsigned char*) pointer_to_misty;
    printf("Nom: %p\n", misty.nom);
    printf("Prenom: %p\n", misty.prenom);
    printf("Favorite letter: %c\n", misty.favorite_letter);
    printf("Age: %d\n", misty.age);
    printf("Taille: %f\n", misty.taille);
    printf("Bytes of misty: ");
    for (size_t i = 0; i < sizeof (struct personne); i++) {
        printf("%02x ", pointer_to_bytes_of_misty[i]);
    }
    printf("\n");
    /*
    nom              96 23 6e 3e 71 55 00 00
    prenom           9d 23 6e 3e 71 55 00 00
    favorite letter  63
    00 00 00 (padding)
    age              09 00 00 00
    taille           3d 0a d7 3e
    00 00 00 00 (padding)


    ___________|___________|___________|___________|
    XX XX XX XX
    AA 00 00 00|XX XX XX XX

    Pour éviter du padding: utiliser les packed structures: __attribute__((packed))
    */

    printf("\n");

    char sur_pile[2];
    printf("Pointeur sur pile: %p\n", sur_pile);
    char* sur_tas = malloc(2);
    printf("Pointeur sur tas: %p\n", sur_tas);
    free(sur_tas);

    printf("\n");

    int tableau_premiers[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
    int* pointer_to_tableau_premiers = tableau_premiers;
    unsigned char* pointer_to_bytes_of_tableau_premiers = (unsigned char*) pointer_to_tableau_premiers;
    printf("Bytes of tableau_premiers: ");
    for (size_t i = 0; i < sizeof tableau_premiers; i++) {
        printf("%02x ", pointer_to_bytes_of_tableau_premiers[i]);
    }
    printf("\n");

    printf("\n");

    char* moi = "Je m’appelle Clémentine !";
    printf("La chaîne: %s\n", moi);
    unsigned char* pointer_to_bytes_of_moi = (unsigned char*) moi;
    printf("Bytes of tableau_premiers: ");
    for (size_t i = 0; i < strlen(moi); i++) {
        printf("%02x ", pointer_to_bytes_of_moi[i]);
    }
    printf("\n");
    /*
    J  e  _  m  ’        a  p  p  e  l  l  e  _  C  l  é     m  e  n  t  i  n  e  __       !
    4a 65 20 6d e2 80 99 61 70 70 65 6c 6c 65 20 43 6c c3 a9 6d 65 6e 74 69 6e 65 e2 80 af 21 
    */

    printf("\n");

    printf("%p\n", main) NOT_A_POINT_VIRGULE

    unsigned char* p = (unsigned char*) main;
    for (size_t i = 0; i < 10; i++) {
        printf("%02x ", p[i]);
    }
    printf("\n");
}