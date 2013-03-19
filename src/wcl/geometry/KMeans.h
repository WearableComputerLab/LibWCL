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

namespace wcl {

	typedef std::vector<wcl::Vector> PointList;

	/**
	 * Class for comparing wcl vectors.
	 * Essentially, we compare x, then y, then z
	 * in less than operations.
	 */
	class VectorCompare {
		public:
			bool operator()(const wcl::Vector& v1, const wcl::Vector& v2);
	};


	/**
	 * A Cluster of points.
	 */
	struct WCL_API Cluster {
		/**
		 * The centroid of the cluster.
		 */
		wcl::Vector mean;

		/**
		 * The points contained in this cluster
		 */
		std::vector<wcl::Vector> points;

		Cluster() : mean(3) {}
	};


	/**
	 * A Class for performing K-Means clustering of a set of 3D points.
	 */
	class WCL_API KMeans {
		public:
			/**
			 * Construct a new KMeans object with a list of points and a number of clusters.
			 *
			 * @param points The list of points.
			 * @param k The number of clusters.
			 */
			KMeans(PointList& points, unsigned k);
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
			const std::vector<Cluster*>& getClusters();

			/**
			 * Returns the cluster that point p belongs to.
			 * Returns NULL if the point is not found, or step/compute have
			 * not yet been called.
			 */
			Cluster* getCluster(wcl::Vector& p);

		private:
			PointList& points;
			unsigned k;
			std::vector<Cluster*> clusters;
			BoundingBox pointBounds;
			std::map<wcl::Vector, Cluster*, VectorCompare> cmap;
	};
};

#endif

