library(deSolve)

data1 = c(5, 2, 3, 1, 10)
data = c(3, 1, 3, 8, 8)

# Sum of squares
sumsquares = (data1 - data2)^2

#  First, the parameters for our model:

params <- c(beta	= .6, gamma = .3, mu = 0.02)  	# contact rate
			
init.values <- c(S = 0.999, I = 0.001, R=0)

times <- seq(0, 150, by = 1)
times
#	Now we can define the differential equation model:

SI <- function(time, y.values, parameters) {  # Don't change this
	with(as.list(c(y.values, parameters)), {  # or this
		
    #eval(parse(text="dS.dt = mu - beta * S * I - mu * S"))
    #eval(parse(text="dI.dt = beta * S * I - gamma * I - mu * I"))
    #eval(parse(text="dR.dt = gamma * I - mu * R"))
		
    eval(parse(text="dS.dt = -1 * beta * S * I"))
    eval(parse(text="dI.dt = beta * S * I - gamma * I"))
    eval(parse(text="dR.dt = gamma * I"))
    
		return(list(c(dS.dt, dI.dt, dR.dt)))		 
	})
}


out <- as.data.frame(ode(func = SI, y = init.values, parms = params, times = times))
							
	#	And now we can make a nice plot of our results:

gdata = rep(0.1, 501)

sumsquares = sum((gdata - out$I) ^ 2)

print("Sum of squares value")
print(sumsquares)

matplot(out$time, out[ ,2:4], type = "l", xlab = "time", ylab = "percent of population", main = "SI Model", lwd = 2)

legend("right", c("Susceptible", "Infectious", "Recovered"), col = 1:3, lty = 1:2)

setGene = function(chromosome, start, length, replacement) {
  chromosome[seq(start, start + length - 1)] = replacement
  
  return(chromosome)
}

generateGene = function(headLength, tailLength, terminals, operators) {
  head = sample(c(terminals, operators, operators, operators, operators), headLength, replace=TRUE)
  tail = sample(terminals, tailLength, replace=TRUE)
  
  return(c(head, tail))
}
        
mutateGene = function(gene, headLength, tailLength, mutationRate, operators, terminals) {
  if(runif(1) < mutationRate) {
    gene = pointMutation(gene, headLength, tailLength, operators, terminals)
  }
  
  return(gene)
}       

mutateThreeGeneChromosome = function(chromosome, headLength, tailLength, mutationRate, operators, terminals) {
  mutated = c(mutateGene(getGene(chromosome, 1, headLength + tailLength), headLength, tailLength, mutationRate, operators, terminals),
    mutateGene(getGene(chromosome, 2, headLength + tailLength), headLength, tailLength, mutationRate, operators, terminals),
    mutateGene(getGene(chromosome, 3, headLength + tailLength), headLength, tailLength, mutationRate, operators, terminals)
  )
  
  return(mutated)
}

pointMutation = function(chromosome, headLength, tailLength, operators, terminals) {
  index = round(runif(1, 0, headLength + tailLength))
  
  if(index < headLength) {
    chromosome[index] = sample(c(operators, terminals), 1)
  }
  else {
    chromosome[index] = sample(terminals, 1)
  }
  
  return(chromosome)
}
        
onePointRecombine = function(chromosome1, chromosome2, headLength, tailLength) {
  recombinationPoint = floor(runif(1, 1, headLength + tailLength))
    
  temp_chromosome = chromosome1

  if(runif(1) < 0.5) {
    chromosome = c(chromosome1[seq(1, recombinationPoint)], chromosome2[seq(recombinationPoint + 1, headLength + tailLength)])
  }
  else {
    chromosome = c(chromosome2[seq(1, recombinationPoint)], temp_chromosome[seq(recombinationPoint + 1, headLength + tailLength)])
  }
  
  return(chromosome)
}

#
# Break a K expression into the constituent rows of an expression tree.
#
buildRows = function(K) {
  rows = list(list())
  row = 1
  next_row_needs = 0
  remaining_needed_for_current_row = 1
  row_index = 1
  
  for(index in seq(1, length(K))) {
    if(K[[index]] == '+' || K[[index]] == '*' || K[[index]] == '-') {
      next_row_needs = next_row_needs + 2
    }
    else {
      
    }
    
    remaining_needed_for_current_row = remaining_needed_for_current_row - 1
    
    rows[[row]] = c(rows[[row]], K[[index]])
    row_index = row_index + 1
    
    #print(paste("Row", row, "Character", K[[index]]))
  
    if(remaining_needed_for_current_row == 0 && next_row_needs == 0) {
      #print("We're done")
      break
    }
      
    if(remaining_needed_for_current_row == 0) {
      row = row + 1
      rows[[row]] = list()
      remaining_needed_for_current_row = next_row_needs
      next_row_needs = 0
    } 
  }
  
  return(rows)
}

#
# Constructs a left to right expression from the rows of an expression tree.
#
buildExpr = function(rows, row, index) {
 
  if(rows[[row]][[index]] == '+' || rows[[row]][[index]] == '-' || rows[[row]][[index]] == '*') {
    # Count the number of operators before the current index
    # to figure out which operator we're currently at (if at all)
    operator_index = sum(rows[[row]][seq(1, index)] == "+") + sum(rows[[row]][seq(1, index)] == "-") + sum(rows[[row]][seq(1, index)] == "*")
    
    # Calculate the index of the left and right children of the current operator
    left_index = (operator_index - 1) * 2 + 1
    right_index = ((operator_index - 1) * 2) + 2
    
    # Recursively build the expression from the left and right children
    left = buildExpr(rows, row + 1, left_index)
    right = buildExpr(rows, row + 1, right_index)
    
    # Return the left child expression plus the current operator plus the right child expression
    return(paste('(', paste(left, rows[[row]][[index]], right), ')', sep=''))
  }
  else {
    return(rows[[row]][[index]])
  }
  
}

#
# Builds the left to right equation from a K expression
#
getExpr = function(K) {
  rows = buildRows(K)
  expr = buildExpr(rows, 1, 1)
}


fitness1 = function(chromosome) {
  a = 1
  b = 2
  c = 3
  
  fitness = abs(15 - eval(parse(text=getExpr(chromosome))))
  
  return(fitness)
}

fitness2 = function(chromosome) {
  print(paste(getExpr(getGene(chromosome, 1, geneLength)), getExpr(getGene(chromosome, 2, geneLength)), getExpr(getGene(chromosome, 3, geneLength)), sep=", "))
  params <- c(beta  = .6, gamma = .3, mu = 0.02)  	# contact rate
  init.values <- c(S = 0.999, I = 0.001, R=0)
  
  times <- seq(0, 150, by = 1)
  
  susceptible_equation = getExpr(getGene(chromosome, 1, headLength + tailLength))
  infectious_equation   = getExpr(getGene(chromosome, 2, headLength + tailLength))
  recovered_equation    = getExpr(getGene(chromosome, 3, headLength + tailLength))
  
  print(susceptible_equation)
  print(infectious_equation)
  print(recovered_equation)
  
  #  Now we can define the differential equation model:
  SI <- function(time, y.values, parameters) {  # Don't change this
  	with(as.list(c(y.values, parameters)), {  # or this
  		if(I > 1 || I < 0) {
        dI.dt = 0
  		}
      else {
        #eval(parse(text="dI.dt = beta * S * I - gamma * I - mu * I"))
        eval(parse(text=paste("dI.dt =", infectious_equation)))
      }
      
      if(S > 1 || S < 0) {
        dS.dt = 0
      }
      else {
        eval(parse(text=paste("dS.dt =", susceptible_equation)))
      }
      
      if(R > 1 || R < 0) {
        dR.dt = 0
      }
      else { 
        eval(parse(text=paste("dR.dt =", recovered_equation)))
      }
  		
  		return(list(c(dS.dt, dI.dt, dR.dt)))		 
  	})
  }
     
  chromosome_data <- as.data.frame(ode(func = SI, y = init.values, parms = params, times = times))
  
  #matplot(chromosome_data$time, chromosome_data[ ,2:4], type = "l", xlab = "time", ylab = "percent of population", main = "SI Model", lwd = 2)
  #Sys.sleep(0)

  # Return the sum of squares between the chromosome results and the time series data
  sumsquares = sum((chromosome_data$I - out$I)^2) + sum((chromosome_data$S - out$S)^2) + sum((chromosome_data$R - out$R)^2)
  
  return(sumsquares)
}

# Gets a gene from a chromosome, assuming that all the genes are the same length
getGene = function(chromosome, geneIndex, geneLength) {
  gene = chromosome[seq((geneIndex - 1) * geneLength + 1, geneIndex * geneLength)]
}
  
        
      
c("*", "*", "*", "-1", "beta", "S", "I", "mu", "mu", "mu", "mu", 
"mu", "mu", "mu", "mu", "mu", "mu", "-", "*", "*", "beta", "*", 
"gamma", "I", "S", "I", "mu", "mu", "mu", "mu", "mu", "mu", "mu", 
"mu", "*", "gamma", "I", "mu", "mu", "mu", "mu", "mu", "mu", 
"mu", "mu", "mu", "mu", "mu", "mu", "mu", "mu")
      
##################
# MAIN ALGORITHM #
##################

headLength = 3
tailLength = 4

geneLength = headLength + tailLength

populationSize = 1000
survivorSize = 500
mutationRate = 0.2

#terminals = c("beta", "gamma", "gamma", "S", "I", "R", "-1")
operators = c("+", "-", "*")

terminals = c("beta * S * I", "gamma * I", "-1")
#operators = c("+", "-")

#
# Generate the initial population
#
population = t(replicate(populationSize, replicate(3, generateChromosome(headLength, tailLength, terminals, operators))))

# Calculate fitnesses
fitnesses = apply(population, 1, fitness2)
fitness_order = order(fitnesses)

best_fitness = min(fitnesses)

best_fitnesses = c(best_fitness)

while(best_fitness > 0.005) {
  
  #
  # Recombination
  #
  
  # Pick the five best individuals
  best_individuals = fitness_order[seq(1, survivorSize)]
  
  for(index in seq(survivorSize, populationSize)) {
    parents = sample(seq(1, survivorSize), 2)
    parent1 = fitness_order[parents[1]]
    parent2 = fitness_order[parents[2]]
    
    
    population[fitness_order[index],] = c(
      onePointRecombine(getGene(population[parent1,], 1, geneLength), getGene(population[parent2,], 1, geneLength), headLength, tailLength),
      onePointRecombine(getGene(population[parent1,], 2, geneLength), getGene(population[parent2,], 2, geneLength), headLength, tailLength),
      onePointRecombine(getGene(population[parent1,], 3, geneLength), getGene(population[parent2,], 3, geneLength), headLength, tailLength)
    )
    #population[fitness_order[index],] = population[fitness_order[parents[1]],]
  }
  
    
  #  
  # Mutation
  #
  population = t(apply(population, 1, mutateThreeGeneChromosome, headLength, tailLength, mutationRate, operators, terminals))
  
  
  #
  # Recalculate fitnesses
  #
  fitnesses = apply(population, 1, fitness2)
  best_fitness = min(fitnesses)
  fitness_order = order(fitnesses)
  
  print(paste("Best: ", best_fitness))
  
  # Record current best fitness
  best_fitnesses = c(best_fitnesses, best_fitness)

  plot(best_fitnesses, type="l")
  Sys.sleep(0)
    
  print(paste("Best so far:", getExpr(population[fitness_order[1],])))
}

plot(best_fitnesses, type="l", xlab="Generation", ylab="Best Fitness", main="Best Fitness vs. Generation")