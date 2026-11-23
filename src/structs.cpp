#include <structs.hpp>

sf::Vector2f nodeToVector2f(Node& node, const int& squareSide){
    return {float(node.pos.col * squareSide), float(node.pos.row * squareSide)};
}