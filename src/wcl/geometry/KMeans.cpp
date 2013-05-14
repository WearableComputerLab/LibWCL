/*
 * Copyright (c) 2013 Michael Marner <michael@20papercups.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <iostream>
#include <limits>
#include <cstdlib>

#include "KMeans.h"

using namespace std;
using namespace wcl;


bool VectorCompare::operator()(const wcl::Vector& v1, const wcl::Vector& v2) {

	return v1[0] < v2[0] || 
		   (v1[0] == v2[0] && v1[1] < v2[1]) ||
		   (v1[0] == v2[0] && v1[1] == v2[1] && v1[2] < v2[2]);
}


KMeans::KMeans(PointList& p, unsigned kk)
 : points(p), k(kk) {

	pointBounds.addPoints(points);
	//clog << "Creating " << k << " clusters" << endl;

	for (int i = 0; i < k; ++i) {
		Cluster* c = new Cluster();
		double xRange = pointBounds.max[0] - pointBounds.min[0];
		c->mean[0] = (rand() %  (int) xRange ) + pointBounds.min[0];
		c->mean[1] = (rand() % ( (int) (pointBounds.max[1] - pointBounds.min[1]))) + pointBounds.min[1];


		double zRange = pointBounds.max[2] - pointBounds.min[2];

		if (zRange != 0) {
			c->mean[2] = (rand() % ( (int) (pointBounds.max[2] - pointBounds.min[2]))) + pointBounds.min[2];
		}
		else {
			c->mean[2] = 0;
		}
		
		clusters.push_back(c);
	}
}


KMeans::~KMeans() {
	cmap.clear();
	std::vector<Cluster*>::iterator cit;
	for (cit = clusters.begin(); cit < clusters.end(); ++cit) {
		delete *cit;
	}
	clusters.clear();
}


unsigned KMeans::step() {
	unsigned swaps = 0;
	std::vector<Cluster*>::iterator cit;
	for (cit = clusters.begin(); cit < clusters.end(); ++cit) {
		(*cit)->points.clear();
		(*cit)->bb.clear();
	}

	for (PointList::iterator pit = points.begin(); pit < points.end(); ++pit) {
		Cluster* closestCluster = NULL;
		double closestDistance = std::numeric_limits<double>::max();

		for (cit = clusters.begin(); cit < clusters.end(); ++cit) {

			double distance = ((*pit) - (*cit)->mean).normal();
			if (distance < closestDistance) {
				closestCluster = *cit;
				closestDistance = distance;
			}
		}

		closestCluster->points.push_back(*pit);
		closestCluster->bb.addPoint(*pit);
		if (cmap.find(*pit) == cmap.end()) {
			//clog << "Adding new mapping" << endl;
			swaps++;
			cmap[*pit] = closestCluster;
		}
		else if (cmap.find(*pit)->second != closestCluster) {
			//clog << "Updating mapping" << endl;
			//clog << "Old Cluster: " << cmap.find(*pit)->second << ", New Cluster: " << closestCluster << endl;
			swaps++;
			cmap[*pit] = closestCluster;
		}
	}
	for (cit = clusters.begin(); cit < clusters.end(); ++cit) {
		if ((*cit)->points.size() > 0) {
			(*cit)->mean[0] = 0;
			(*cit)->mean[1] = 0;
			(*cit)->mean[2] = 0;
			for (PointList::iterator pit = (*cit)->points.begin(); pit < (*cit)->points.end(); ++pit) {
				(*cit)->mean = (*cit)->mean + *pit;
			}
			(*cit)->mean = (*cit)->mean / (*cit)->points.size();
		}
	}
	
	return swaps;
}

void KMeans::compute() {
	while (step() > 0) {}
}

const std::vector<Cluster*>& KMeans::getClusters() {
	return clusters;
}


Cluster* KMeans::getCluster(wcl::Vector& p) {
	if (cmap.find(p) == cmap.end())
		return NULL;
	else
		return cmap[p];

}
