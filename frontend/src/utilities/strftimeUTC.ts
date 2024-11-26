function pad(num) {
  if (num < 10) {
    return '0' + num;
  }
  return num;
}
  

function padMilliseconds(num) {
  if (num < 10) {
    return '0' + num;
  }
  else if (num >= 10 && num < 100) {
    return '00' + num;
  }
  return num;
}
  
function strftimeUTC(format, date) {

  const replacements = {
    '%Y': date.getUTCFullYear(),
    '%m': pad(date.getUTCMonth() + 1), // JavaScript months are 0-indexed
    '%d': pad(date.getUTCDate()),
    '%H': pad(date.getUTCHours()),
    '%M': pad(date.getUTCMinutes()),
    '%S': pad(date.getUTCSeconds()),
    '%f': padMilliseconds(date.getMilliseconds())
  };

  for (const key in replacements) {
    format = format.replace(key, replacements[key]);
  }
  return format;
}

export default strftimeUTC;
