
{ /* Defines the user's access level */ }
export enum Access {
  Denied,
  ReadOnly,
  ReadWrite
};

{ /* Defines the user's credentials */ }
export interface UserCredentials {
  name: string;
  password: string;
  jsonWebToken: string; 
  permissions: Access;
  authenticated: boolean;
};

