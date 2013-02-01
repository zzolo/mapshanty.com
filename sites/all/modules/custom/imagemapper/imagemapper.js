// $Id$

// Replace to override marker creation
Drupal.gmap.createImageMapperMarker = function(opts) {
  // icon
  var icon = new GIcon();
  icon.image = opts.imageurl;
  icon.iconSize = new GSize(opts.iconsize.x, opts.iconsize.y);
  icon.iconAnchor = new GPoint(opts.iconanchor.x, opts.iconanchor.y);
  icon.infoWindowAnchor = new GPoint(opts.iconwindowanchor.x, opts.iconwindowanchor.y);
  // add point
  var point = new GLatLng(opts.latitude, opts.longitude);
  // add marker
  var marker = new GMarker(point, icon);
  // add info window text and event
  GEvent.addListener(marker, "click", function() {
    marker.openInfoWindowHtml(opts.text);
  });

  // return marker
  return marker;
}

// Add handler for imagemapper
Drupal.gmap.addHandler('gmap', function(elem) {
  var obj = this;
  // Check for imagemapper
  if (Drupal.imagemapper.markers) {
    var imarkers = Drupal.imagemapper.markers;
    // When icons are ready
    obj.bind('ready', function() {
      // Go through markers
      for (i=0; i<imarkers.length; i++) {
        // Get this marker
        var marker = Drupal.gmap.createImageMapperMarker(imarkers[i]);

        obj.map.addOverlay(marker);
      }
    });
  }
});
