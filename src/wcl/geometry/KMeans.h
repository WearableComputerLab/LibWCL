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


#ifndef LIBWCL_KMEANS_H
#define LIBWCL_KMEANS_H

#include <map>
#include <vector>
#include <wcl/maths/Vector.h>
#include <wcl/geometry/BoundingBox.h>
#include <iostream>
#include <limits>
#include <cstdlib>

namespace wcl {

	class WCL_API Position {
		public:
			virtual double x() const = 0;
			virtual double y() const = 0;
			virtual double z() const = 0;
            virtual ~Position(){}
	};

	/**
	 * A Cluster of points.
	 */
	template <class PositionType>
	class WCL_API Cluster {
		public:
			/**
			 * The centroid of the cluster.
			 */
			wcl::Vector mean;

			/**
			 * The points contained in this cluster
			 */
			std::vector<PositionType*> points;

			Cluster() : mean(3) {}

			BoundingBox bb;
	};


	/**
	 * Class for comparing wcl vectors.
	 * Essentially, we compare x, then y, then z
	 * in less than operations.
	 */
	template <class PositionType>
	class PositionCompare {
		public:
			bool operator()(const PositionType& v1, const PositionType& v2);
			bool operator()(const PositionType* v1, const PositionType* v2);
	};




	/**
	 * A Class for performing K-Means clustering of a set of 3D points.
	 */

	template <class PositionType, class ClusterType>
	class WCL_API KMeans {
		public:
			/**
			 * Construct a new KMeans object with a list of points and a number of clusters.
			 *
			 * @param points The list of points.
			 * @param k The number of clusters.
			 */
			KMeans(std::vector<PositionType *>& points, unsigned k);
			virtual ~KMeans();

			/**
			 * Runs the algorithm until it converges on a solution.
			 * This is the equivalent of running step() repeatedly
			 * until there are no more stops.
			 */
			void compute();

			/**
			 * Performs a single step of the clustering algorithm.
			 * Useful if you want to see this work step by step.
			 *
			 * @return The number of swaps performed in this step.
			 */
			unsigned step();

			/**
			 * Returns the clusters.
			 */
			const std::vector<ClusterType*>& getClusters();

			/**
			 * Returns the cluster that point p belongs to.
			 * Returns NULL if the point is not found, or step/compute have
			 * not yet been called.
			 */
			ClusterType* getCluster(PositionType* p);

		private:
			std::vector<PositionType*>& points;
			unsigned k;
			std::vector<ClusterType*> clusters;
			BoundingBox pointBounds;
			std::map<PositionType*, ClusterType*, PositionCompare<PositionType> > cmap;

			void randomInitialisation();
			void forgyInitialisation();
	};

	template <class PositionType>
		bool PositionCompare<PositionType>::operator()(const PositionType& v1, const PositionType& v2) {
			return v1.x() < v2.x() || 
				(v1.x() == v2.x() && v1.y() <  v2.y()) ||
				(v1.x() == v2.x() && v1.y() == v2.y() && v1.z() < v2.z());
		}

	template <class PositionType>
		bool PositionCompare<PositionType>::operator()(const PositionType* v1, const PositionType* v2) {
			return this->operator()(*v1, *v2);
		}


	template <class PositionType, class ClusterType>
		KMeans<PositionType, ClusterType>::KMeans(std::vector<PositionType*>& p, unsigned kk)
		: points(p), k(kk) {
			forgyInitialisation();
		}

	template <class PositionType, class ClusterType>
	void KMeans<PositionType, ClusterType>::randomInitialisation() {
			typedef std::vector<PositionType*> PointList;
			for (typename PointList::iterator it = points.begin(); it < points.end(); ++it) {
				PositionType* pos = *it;
				pointBounds.addPoint(wcl::Vector(pos->x(), pos->y(), pos->z()));
			}
			//clog << "Creating " << k << " clusters" << endl;

			for (int i = 0; i < k; ++i) {
				ClusterType * c = new ClusterType ();
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

	template <class PositionType, class ClusterType>
	void KMeans<PositionType, ClusterType>::forgyInitialisation() {
		while (clusters.size() < k) {
			PositionType* p = points[rand() % points.size()];
			if (getCluster(p) == NULL) {
				ClusterType * c = new ClusterType ();
				c->mean[0] = p->x();
				c->mean[1] = p->y();
				clusters.push_back(c);
			}
		}
	}


	template <class PositionType, class ClusterType>
		KMeans<PositionType, ClusterType>::~KMeans() {
			cmap.clear();
			typename std::vector<ClusterType*>::iterator cit;
			for (cit = clusters.begin(); cit < clusters.end(); ++cit) {
				delete *cit;
			}
			clusters.clear();
		}


	template <class PositionType, class ClusterType>
		unsigned KMeans<PositionType, ClusterType>::step() {
			unsigned swaps = 0;
			typename std::vector<ClusterType*>::iterator cit;
			typedef std::vector<PositionType*> PointList;

			for (cit = clusters.begin(); cit < clusters.end(); ++cit) {
				(*cit)->points.clear();
				(*cit)->bb.clear();
			}

			for (typename PointList::iterator pit = points.begin(); pit < points.end(); ++pit) {
				ClusterType* closestCluster = NULL;
				double closestDistance = std::numeric_limits<double>::max();

				for (cit = clusters.begin(); cit < clusters.end(); ++cit) {

					double distance = (wcl::Vector((*pit)->x(),(*pit)->y(),(*pit)->z()) - (*cit)->mean).normal();
					if (distance < closestDistance) {
						closestCluster = *cit;
						closestDistance = distance;
					}
				}

				closestCluster->points.push_back(*pit);
				closestCluster->bb.addPoint(wcl::Vector((*pit)->x(),(*pit)->y(),(*pit)->z()));
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
					for (typename PointList::iterator pit = (*cit)->points.begin(); pit < (*cit)->points.end(); ++pit) {
						(*cit)->mean = (*cit)->mean + wcl::Vector((*pit)->x(), (*pit)->y(), (*pit)->z()) ;
					}
					(*cit)->mean = (*cit)->mean / (*cit)->points.size();
				}
			}

			return swaps;
		}

	template <class PositionType, class ClusterType>
		void KMeans<PositionType, ClusterType>::compute() {
			while (step() > 0) {}
		}

	template <class PositionType, class ClusterType>
		const std::vector<ClusterType*>& KMeans<PositionType, ClusterType>::getClusters() {
			return clusters;
		}


	template <class PositionType, class ClusterType>
		ClusterType* KMeans<PositionType, ClusterType>::getCluster(PositionType* p) {
			if (cmap.find(p) == cmap.end())
				return NULL;
			else
				return cmap[p];

		}
};

#endif

