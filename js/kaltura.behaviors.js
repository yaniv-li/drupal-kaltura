(function ($) {
  Drupal.behaviors.kaltura = {
    attach: function(context, settings) {
      console.dir(context);
      $('.remove_media', context).click( function () {
        $(this).parents('.kaltura-thumb-wrap').nextAll().children('input:hidden').val('');
        $(this).parents('.kaltura-thumb-wrap').replaceWith('');
        });
      }
    }
  })(jQuery);
