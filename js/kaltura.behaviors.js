(function ($) {
  Drupal.behaviors.kaltura = {
    attach: function(context, settings) {
      $('.remove_media', context).click( function () {
        $(this).parents('.kaltura-thumb-wrap').nextAll().children('input:hidden').val('');
        $(this).parents('.kaltura-thumb-wrap .kaltura_field_thumb').replaceWith('');
        });
      }

    }
    Drupal.behaviors.kaltura_add = {
      attach: function (context, settings) {
        $('.kentry_add', context).click( function () {
          var field_name = $(this).attr('name');
          var entry_id = $(this).attr('id');
          var media_type = $(this).val();
          var src = $(this).prev().find('img').attr('src');
          var t = '<div class="kaltura_field_thumb"><img src="' + src + '"/><br/> <input type="button" title="remove item" class="remove_media" /></div>';
          var mtselect = "#" + field_name + "-media-type input";
          var etselect = "#" + field_name + "-entryid input";
          var thumb_select = "#" + field_name + "-thumb-wrap .kaltura_field_thumb";
          $(etselect, window.parent.document).val(entry_id);
          $(mtselect, window.parent.document).val(media_type);
          $(thumb_select, window.parent.document).replaceWith(t);
          window.top.Drupal.attachBehaviors();
          window.top.kalturaCloseModalBox();
          });
        }
      }
  })(jQuery);
