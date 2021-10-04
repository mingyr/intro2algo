
#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <random>
#include <cmath>
#include <functional>
#include <numeric>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

static const int N = 22;
static random_device rd;

inline double geno2pheno(unsigned long n, double lower, double upper)
{
	return (lower + (upper - lower) / ((2 << N) - 1) * n);
}

struct objective
{
	using objective_fn = function<double(unsigned long n)>;
	objective(objective_fn dec) : dec_{ std::move(dec) } {}
	objective_fn dec_;
};

vector<double> fitness(vector<bitset<N>>& population, objective& obj )
{
	// cout << "In function: " << __func__ << endl;

	vector<double> scores;
	scores.resize(population.size());
	transform(population.begin(), population.end(), scores.begin(), [obj](bitset<N>& bs) {
		return obj.dec_(bs.to_ulong());
		});

	return scores;
}

vector<int> selection(vector<double>& scores)
{
	// cout << "In function: " << __func__ << endl;

	double min = *min_element(scores.begin(), scores.end());

	for_each(scores.begin(), scores.end(), [min](auto& n) { n -= min; });

	vector<double> boundary;
	boundary.resize(scores.size());

	double accu = 0;
	transform(scores.begin(), scores.end(), boundary.begin(), [&accu](auto& v) { return accu += v;  });

	static mt19937 mt(rd());
	uniform_real_distribution<double> dist(0.0, accu);

	vector<int> idx;

	/*
	cout << "scores: " << endl;
	for_each(scores.cbegin(), scores.cend(), [](auto n) { cout << n << " ";  });
	cout << endl;

	cout << "boundary: " << endl;
	for_each(boundary.cbegin(), boundary.cend(), [](auto n) { cout << n << " ";  });
	cout << endl;
	*/

	for (auto i = scores.size(); i > 0; i--)
	{
		double p = dist(mt);
		// cout << "p = " << p << endl;

		for (auto it = boundary.begin(); it != boundary.end(); it++)
		{
			if (p <= *it)
			{
				idx.push_back(static_cast<int>(distance(boundary.begin(), it)));
				break;
			}
		}
	}

	// cout << "idx size: " << idx.size() << endl;

	return idx;
}

bool crossover(vector<bitset<N>>& population, int crossover_pairs)
{
	// cout << "In function: " << __func__ << endl;

	static mt19937 mt(rd());
	static uniform_int_distribution<int> ind_dist(0, static_cast<int>(population.size()) - 1);
	static uniform_int_distribution<int> pos_dist(0, N - 1);

	for (int i = 0; i < crossover_pairs; i++)
	{
		int i1 = ind_dist(mt);
		int i2 = ind_dist(mt);

		// cout << "i1 = " << i1 << "; i2 = " << i2 << endl;

		bitset<N>& bs1 = population[i1];
		bitset<N>& bs2 = population[i2];

		for (auto i = pos_dist(mt); i != N; i++)
		{
			bs1[i] = bs1[i] ^ bs2[i];
			bs2[i] = bs1[i] ^ bs2[i];
			bs1[i] = bs1[i] ^ bs2[i];
		}
	}

	return true;
}

bool mutation(vector<bitset<N>>& population, int mutation_indivs)
{
	// cout << "In function: " << __func__ << endl;

	static mt19937 mt(rd());

	static uniform_int_distribution<int> ind_dist(0, static_cast<int>(population.size()) - 1);
	static uniform_int_distribution<int> pos_dist(0, N - 1);

	for (int i = 0; i < mutation_indivs; i++)
		population[ind_dist(mt)].flip(pos_dist(mt));

	return true;
}

typedef struct ga_config
{
	int population_size;
	int generations;
	double tolerance;
	double crossover_ratio;
	double mutation_ratio;
} ga_config;

bitset<N> genetic_algorithm(ga_config& gac, objective& obj)
{
	// Initialize population

	vector<bitset<N>> population;

	double p = 0.5;
	mt19937 gen(rd());
	bernoulli_distribution d(p);

	for (int i = 0; i < gac.population_size; i ++)
	{
		bitset<N> bits;
		for (int n = 0; n < N; n ++)
		{
			bits[n] = d(gen);
		}

		population.push_back(move(bits));
	}

	double abs_error, max_score_last;
	bool first_generation = true;
	vector<double> scores;
	vector<int> idx;

	do
	{
		// Fitness assessment for reproduction	
		scores = fitness(population, obj);
		double max_score = *max_element(scores.begin(), scores.end());

		idx = selection(scores);
		vector<bitset<N>> new_population;

		for (auto it = idx.begin(); it != idx.end(); it++)
		{
			new_population.push_back(*next(population.begin(), *it));
		}

		population = move(new_population);

		// cout << "population size" << population.size() << endl;

		// crossover
		crossover(population, int(gac.population_size * gac.crossover_ratio));

		// mutation
		mutation(population, int(gac.population_size * gac.mutation_ratio));

		if (first_generation)
		{
			max_score_last = max_score;
			first_generation = false;
			abs_error = DBL_MAX;
		}
		else
		{
			abs_error = abs(max_score_last - max_score);
			max_score_last = max_score;
		}
		
		gac.generations--;

	} while (gac.generations > 0 && abs_error > gac.tolerance);

	return population[distance(scores.begin(), max_element(scores.begin(), scores.end()))];
}


void genetic_algorithm_test()
{
	double lower = -1.0;
	double upper = 2.0;
	objective obj{ [lower, upper](unsigned long n) {
		auto x = geno2pheno(n, lower, upper);
		return x * sin(10 * M_PI * x) + 2; } };

	ga_config gac;
	gac.population_size = 100;
	gac.generations = 1000;
	gac.tolerance = 0.0001;
	gac.crossover_ratio = 0.8;
	gac.mutation_ratio = 0.04;

	bitset<N> optimal = genetic_algorithm(gac, obj);

	cout << "The optimal sulotion to the problem is " << geno2pheno(optimal.to_ulong(), lower, upper) << endl;
	cout << "The value of the objective function is " << obj.dec_(optimal.to_ulong()) << endl;
}






