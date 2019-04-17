#include <SFML/Graphics.hpp>
#include <iostream>
#include <complex>


sf::CircleShape make_circle(double radius, double xpos, double ypos)
{
    /*
        Returns a circle object with a specified radius at the specified position

        Arguments:
            -radius -- circle radius
            -xpos   -- x position in pixels
            -ypos   -- y position in pixels
    */

    sf::CircleShape circle;
    circle.setRadius(radius);
    circle.setPosition(xpos - radius, ypos - radius);
    circle.setOutlineThickness(1.0);
    circle.setOutlineColor(sf::Color(255, 0, 255));
    circle.setFillColor(sf::Color::Transparent);

    return circle;
}


std::complex<double> compute_coef(std::vector<std::complex<double>> points, double k)
{
    /*
        Computes the k-th complex Fourier coefficient C_k

        Arguments:
            -points -- vector of complex numbers corresponding to the location of image pixels (scaled down to real numbers)
            -k      -- index specifying the k-th Fourier coefficient
    */

    std::complex<double> result;
    double dt = 2*3.1415926535 / points.size(); // time step for integration
    double t = 0.0;

    // integration
    for (int j=0; j<points.size(); j++)
    {
        result += points[j]*exp(- std::complex<double>(0, 1.0) * k * t)*dt;
        t += dt;
    }

    return 1.0/(2*3.1415926535)*result;
}

int main()
{
    int height = 600;
    int width = 600;
    int unit = 50;      // 1 in real number space = 1*unit in pixel space
    sf::RenderWindow window(sf::VideoMode(width, height), "Epicycles");

    sf::Color col;
    sf::VertexArray image_vertices(sf::Points);
    sf::Vertex image_vertex;

    std::vector<std::complex<double>> points;

    int N = 100;        // use 2*N epicycles (+1 anchor point)
    std::complex<double> C[2*N+1];      // complex Fourier coefficients
    std::complex<double> Z[2*N+1];      // Z[i] is the complex point of the i-th epicycle (C[0]*exp() + ... +C[i] * exp(..))

    sf::CircleShape circle;
    std::vector<sf::Vertex> vertices_radii;
    std::vector<sf::Vertex> trail;

    double t = 0.0;
    double dt;
    double f = 1.0/2;   // controls how frequently epicycles traverse the points
    bool done_drawing = false;

    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (done_drawing == false)
            {
            if (event.type == sf::Event::MouseMoved)
            {
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                    {
                    image_vertex.position = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
                    image_vertex.color = sf::Color::White;
                    image_vertices.append(image_vertex);

                    points.push_back({image_vertex.position.x/unit - width/(2*unit), height/(2*unit) - image_vertex.position.y/unit});
                    }
            }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                done_drawing = true;
                for (int n=0; n<2*N+1; n++)
                {
                    double k = pow(-1, n+1)*ceil(n/2.0);
                    C[n] = compute_coef(points, k);
                }

                Z[0] = C[0];
                dt = 2*3.1415926535 / points.size();
            }
        }

        if (done_drawing == true)
        {
        for (int i=1; i<2*N+1; i++)
        {
            Z[i] = Z[i-1] + C[i]*exp(pow(-1, i+1)*ceil(i/2.0) * f * std::complex<double>(0, 1) * t);
        }

        for (int i=0; i<2*N+1; i++)
        {
            vertices_radii.push_back(sf::Vertex(sf::Vector2f(width/2 + Z[i].real()*unit, height/2-Z[i].imag()*unit)));
        }
        trail.push_back(sf::Vertex(sf::Vector2f(width/2 + Z[2*N+1 -1].real()*unit, height/2-Z[2*N+1 -1].imag()*unit), sf::Color::Green));
        }

        window.clear(sf::Color::Black);
        window.draw(image_vertices);

        if (done_drawing == true)
        {
        for (int i=1; i<2*N+1; i++)
        {
            circle = make_circle(std::abs(C[i])*unit, vertices_radii[i-1].position.x, vertices_radii[i-1].position.y);
            window.draw(circle);
        }

         window.draw(&trail[0], trail.size(), sf::LineStrip);
         window.draw(&vertices_radii[0], 2*N+1, sf::LineStrip);

         vertices_radii.clear();
         t += dt;
        }

        window.display();
        window.setFramerateLimit(500);
    }

    return 0;
}
