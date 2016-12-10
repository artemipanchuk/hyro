define([
	"text!templates/ui.html",
	"css!styles/ui"
], function(tmplUI) {
	this.UI = (function() {
		var wrapper,
				camera, scene, renderer,
				geometry, material, mesh,
				quaternion, socket;

		function UI () {
			$.templates({
				tmplUI: tmplUI
			});
		}

		UI.prototype.initWebGL = function(wrapperSelector) {
			wrapper = $(wrapperSelector);

			// Camera stetup
			camera = new THREE.PerspectiveCamera(75, wrapper.width() / wrapper.height(), 1, 1000);
			camera.position.z = 500;
			
			// Scene setup
			scene = new THREE.Scene();
			
			// Object setup
			geometry = new THREE.IcosahedronGeometry(200, 1);
			material = new THREE.MeshBasicMaterial({
				color:              0xfff999fff,
				wireframe:          true,
				wireframelinewidth: 10
			});

			mesh = new THREE.Mesh(geometry, material);
			mesh.position.y = 50;

			scene.add(mesh);
			
			// Renderer setup
			renderer = new THREE.CanvasRenderer();
			renderer.setSize(wrapper.width(), wrapper.height());
			
			$('#webglSection').append(renderer.domElement);

			renderer.render(scene, camera);
		}

		UI.prototype.updateWebGL = function(x, y, z, w) {
			var quaternion = new THREE.Quaternion(x, y, z, w);

			mesh.matrix.setRotationFromQuaternion(quaternion);

			renderer.render( scene, camera);
		}

		return UI;
	})();
});