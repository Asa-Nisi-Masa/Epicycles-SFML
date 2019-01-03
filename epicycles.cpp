#include <iostream>
#include <SFML/Graphics.hpp>
#include <complex>
#include <math.h>

sf::CircleShape make_circle(double radius, double xpos, double ypos)
{
    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setPosition(xpos - radius, ypos - radius);
    circle.setOutlineThickness(2.0);
    circle.setFillColor(sf::Color::Transparent);

    return circle;
}

sf::Vertex* make_line(double x1, double y1, double x2, double y2)
{
    sf::Vertex* line = new sf::Vertex[2];
    line[0] = sf::Vertex(sf::Vector2f(x1, y1));
    line[1] = sf::Vertex(sf::Vector2f(x2, y2));
    return line;
}

int main()
{
    int height = 600;
    int width = 600;
    int unit = 50;
    sf::RenderWindow window(sf::VideoMode(width, height), "Epicycles");


    sf::Image image;
    if(!image.loadFromFile("image.png"))
        return -1;

    int imagex = image.getSize().x;
    int imagey = image.getSize().y;
    sf::Color col;
    sf::VertexArray image_vertices(sf::Points);
    sf::Vertex image_vertex;

//    int foobar[imagey*imagex][3]; // out of memory

    for (int yy=0;yy<imagey;yy++)
        {
            for (int xx=0;xx<imagex;xx++)
            {
            col = image.getPixel(xx, yy);
            image_vertex.position = sf::Vector2f(xx, yy);
            image_vertex.color = col;
            image_vertices.append(image_vertex);
            }
        }


    int N = 500;
    std::complex<double> C[N];

    for (int n=1; n<N; n++)
    {
        C[n] = {1.0/(n), 0}; // arbitrary coefficients
    }


    std::complex<double> Z[N];
    Z[0] = C[0];

    double t = 0;
    sf::CircleShape circle;
    std::vector<sf::Vertex> vertices;
    std::vector<sf::Vertex> trail;
    double f = 0.1;
    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        for (int i=1; i<N; i++)
        {
            Z[i] = Z[i-1] + C[i]*exp(i* f*2*3.141592*std::complex<double>(0, 1) * t);
        }

        for (int i=0; i<N; i++)
        {
            vertices.push_back(sf::Vertex(sf::Vector2f(width/2 + Z[i].real()*unit, height/2-Z[i].imag()*unit)));
        }
        trail.push_back(sf::Vertex(sf::Vector2f(width/2 + Z[N-1].real()*unit, height/2-Z[N-1].imag()*unit), sf::Color::Green));

        window.clear(sf::Color::Black);

        for (int i=1; i<N; i++)
        {
            circle = make_circle(std::abs(C[i])*unit, width/2 + Z[i-1].real()*unit, height/2-Z[i-1].imag()*unit);
            window.draw(circle);
        }

        window.draw(image_vertices);
        window.draw(&trail[0], trail.size(), sf::LineStrip);
        window.draw(&vertices[0], N, sf::LineStrip);

        window.display();
        window.setFramerateLimit(30);

        vertices.clear();
        t += 1.0/30;
    }


    return 0;
}
