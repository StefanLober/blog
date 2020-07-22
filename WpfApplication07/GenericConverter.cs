using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using System.Windows.Media;
using System.Linq;

namespace Local
{
    public class GenericConverter<TKey, TValue> : DependencyObject, IValueConverter
    {
        public static readonly DependencyProperty ValuesProperty =
            DependencyProperty.RegisterAttached("Values", typeof(Dictionary<TKey, TValue>), typeof(GenericConverter<TKey, TValue>));

        public static void SetValues(DependencyObject obj, Dictionary<TKey, TValue> value)
        {
            obj.SetValue(ValuesProperty, value);
        }

        public static Dictionary<TKey, TValue> GetValues(DependencyObject obj)
        {
            return (Dictionary<TKey, TValue>)obj.GetValue(ValuesProperty);
        }

        public GenericConverter()
        {
            SetValues(this, new Dictionary<TKey, TValue>());
        }

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var castedValue = (TKey)value;

            if (targetType != typeof(TValue) || castedValue == null || !GetValues(this).ContainsKey(castedValue))
                return default(TValue);

            return GetValues(this)[(TKey)value];
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var castedValue = (TValue)value;

            if (targetType != typeof(TKey) || castedValue == null || !GetValues(this).ContainsValue(castedValue))
                return null;

            return GetValues(this).FirstOrDefault(x => x.Value.Equals(castedValue)).Key;
        }
    }

    public class BrushConverter : GenericConverter<DisplayValueEnum, Brush> { }
}
