#include "image_generator.hpp"

namespace gro4t {

ImageGenerator::ImageGenerator(const ImageGeneratorConfig& config)
    : config(config), state(config), generator(std::random_device{}()), real_dist(0.0, 1.0) {}

void ImageGenerator::nextGeneration() {
    state.nextGeneration();
    if (state.nextCircle()) {
        state.current_circle_generation = 0;
        state.current_circle = (state.current_circle + 1) % config.image_props.circles_num;
        state.sigma = 1.0;
    }
    auto new_generated_image = getGeneratedImage();
    if (new_generated_image.getCirclesNum() < config.image_props.circles_num &&
        new_generated_image.getCirclesNum() <= state.current_circle)
        new_generated_image.addCircle();
    else
        new_generated_image.mutate(state.current_circle, state.sigma);
    new_generated_image.evaluate(config.original_image);
    if (new_generated_image.getFitnessScore() > getGeneratedImage().getFitnessScore()) {
        state.generated_image = new_generated_image;
        state.result_table.push_back(true);
    } else
        state.result_table.push_back(false);
    if (--state.generations_since_last_evaluation == 0) {
        state.generations_since_last_evaluation = config.generations_per_evaluation;
        int successes = std::count(state.result_table.begin(), state.result_table.end(), true);
        double success_rate = successes / (double)config.generations_per_evaluation;
        if (success_rate > 0.2) state.sigma = 1.22 * state.sigma;
        if (success_rate < 0.2) state.sigma = 0.82 * state.sigma;
    }
    displayLastGenerationInfo();
}

GeneratedImage& ImageGenerator::getGeneratedImage() { return state.generated_image; }

void ImageGenerator::displayLastGenerationInfo() {
    std::cout << "generation: " << state.generation << std::endl;
    std::cout << " score: " << getGeneratedImage().getFitnessScore() << std::endl;
}

}  // namespace gro4t