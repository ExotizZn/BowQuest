#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "camera.h"

typedef struct {
    float x, y;
} Point;

// Dessiner un rectangle
void fillRect(SDL_Renderer *renderer, float x, float y, float w, float h);

// Dessiner un rectangle avec les bords arrondis
void roundRect(SDL_Renderer *renderer, float x, float y, float w, float h, float radius, SDL_Color color);

// Dessiner un rectangle avec une inclinaison
void drawFilledRotatedRect(SDL_Renderer *renderer, float x, float y, float w, float h, float angle, SDL_Color color);

// Dessiner une ligne
void drawLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, SDL_Color color);

// Dessiner un cercle (contour)
void circle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color);

// Dessiner un cerlce rempli
void filledCircle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color);

// Charger une image depuis un fichier .h
SDL_Surface *CreateSurfaceFromMemory(const unsigned char *data, size_t size);

// Dessiner une grille
void drawGrid(SDL_Renderer *renderer, Camera *camera, int w, int h);

// Dessiner le fond du jeu
void drawBackground(SDL_Renderer *renderer, Camera *camera, int w, int h);

// Renvoie une structure "SDL_Color" avec les couleurs
SDL_Color RGBA(int r, int g, int b, int a);

// Dessiner un texte
void drawText(void *data, const char *text, TTF_Font * font, float x, float y, SDL_Color color, bool centered);

// Dessiner un texte avec retour à la ligne
void drawTextWrapped(void *data, const char *text, TTF_Font *font, float x, float y, SDL_Color color, int wrap_width, bool centered);

// Test : Fonction de Bezier Quadratic
Point bezier_quadratic(Point p0, Point p1, Point p2, float t);

#endif