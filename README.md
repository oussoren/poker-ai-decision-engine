# poker-ai-decision-engine

**Status:** Active Development (Core Engine Complete)
**Language:** C++ (ISO C++17 Standard) (Moving to Python soon)
**Focus:** Game Theory, Probability, Reinforcement Learning

### Project Overview
This project is a high-performance C++ implementation of a Texas Hold'em game engine, designed as a testbed for exploring **Game Theory Optimal (GTO)** strategies and decision-making under uncertainty.

While many poker bots rely on static heuristics, the goal of this engine is to eventually support **Reinforcement Learning (RL)** agents that can adapt to adversarial conditions by modeling opponent behavior and optimizing Expected Value (EV) over sequential decision nodes.

### Current Features (Core Engine)
* **Game State Management:** Complete implementation of Texas Hold'em rules, including street transitions (Pre-flop, Flop, Turn, River), pot logic, and betting rounds.
* **Hand Evaluation:** Algorithms to calculate hand strength and win probabilities against random distributions.
* **Probability-Based Bot:** A baseline agent that makes decisions (Fold/Call/Raise) based on immediate pot odds and equity calculations.
* **C++ Optimization:** Utilizes efficient memory management and data structures for rapid Monte Carlo hand simulations.

### Roadmap (Q1 2026 Objectives)
This project is currently in the "Intelligence Integration" phase. The base game logic is stable, and development is shifting toward advanced decision modeling.

* [ ] **Opponent Modeling (Classifier):** Implementing a classifier to categorize opponent playstyles (e.g., Tight-Aggressive, Loose-Passive) based on betting history.
* [ ] **Reinforcement Learning Integration:** Developing an RL interface to train agents using self-play, moving beyond static probability tables.
* [ ] **GTO Approximation:** Refining the decision engine to approximate Nash Equilibrium strategies in heads-up scenarios.

### Building & Running
This project uses the Qt framework.

**Prerequisites:**
* Qt 5 or Qt 6
* qmake (usually installed with Qt)

**Compilation:**
```bash
# Clone the repository
git clone [https://github.com/oussoren/poker-ai-decision-engine.git](https://github.com/oussoren/poker-ai-decision-engine.git)

# Navigate to directory
cd poker-ai-decision-engine

# Generate Makefile from project file
qmake pk_bot.pro

# Compile
make

# Run
./pk_bot
